#include "demon/db.hpp"
#include "demon/ram_storage.hpp"
#include "common/logger.hpp"
#include "common/time.hpp"

#include <sqlite3.h>

#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include <ctime>
#include <type_traits>

// does move Tables name to defines - good idea?

#define USERS_TABLE "Users"
#define APP_TABLE "Applications"
#define REC_TABLE "Records"
#define CATEGOR_TABLE "Categories"

static const char* CREATE_TABLES = "CREATE TABLE IF NOT EXISTS " USERS_TABLE " (" \
						"user_id INTEGER PRIMARY KEY," \
						"user_name TEXT"
				");" \

				"CREATE TABLE IF NOT EXISTS " APP_TABLE " (" \
						"app_id INTEGER PRIMARY KEY," \
						"type_id INTEGER," \
						"app_name TEXT," \
                        "app_alias TEXT," \
						"FOREIGN KEY(type_id) REFERENCES " CATEGOR_TABLE "(type_id)"
				");" \

				"CREATE TABLE IF NOT EXISTS " REC_TABLE " (" \
									/*"rec_id INTEGER PRIMARY KEY," \ */
						"user_id INTEGER," \
						"app_id INTEGER," \
						"rec_time DATETIME," \
						"uptime INTEGER," \
                        "describe TEXT," \
						"FOREIGN KEY(user_id) REFERENCES " USERS_TABLE "(user_id)," \
						"FOREIGN KEY(app_id) REFERENCES " APP_TABLE "(app_id)"
				");" \

				"CREATE TABLE IF NOT EXISTS " CATEGOR_TABLE "(" \
						"type_id INTEGER PRIMARY KEY," \
						"type_name TEXT" \
				");";


static const char sqlInsertRecord[] = "INSERT INTO " REC_TABLE "(" \
					    "user_id, app_id, rec_time, uptime, describe) " \
					    "VALUES (1, ?1, ?2, ?3, ?4);";


static const char sqlGetAppId[] = "SELECT app_id FROM " APP_TABLE " WHERE app_name = ?1;";
static const char sqlGetAppName[] = "SELECT app_name FROM " APP_TABLE " WHERE app_id = ?1;";

static const char sqlInsertApp[] = "INSERT INTO " APP_TABLE " (app_name) VALUES (?1);";

static const char sqlGetCount[] = "SELECT COUNT(*) FROM " REC_TABLE;

// create tables if not
static void checkTables( sqlite3* db ) {
	char* zErrMsg = nullptr;
	int rc;

	rc = sqlite3_exec( db, CREATE_TABLES, nullptr, 0, &zErrMsg );

	if( rc != SQLITE_OK ) 
		throw std::runtime_error(zErrMsg);
}

// sqlite3_config befor any connection.
// in GUI version, i have to use v2 and set flag read_only
// mb i need here NOMUTEX for correct communication
Database::Database( const char* dbName ) {
	if( sqlite3_open( dbName, &_db ) != SQLITE_OK )
		throw std::runtime_error("Error while starting sqlite3");

	checkTables( _db );
}

Database::Database( const std::string& dbName, std::true_type createInHomeDir ) {
    std::string homePath = std::getenv( "HOME" );
    homePath.push_back('/');
    homePath.append( dbName );

    logger.log(LogLvl::Info, "DB path: ", homePath );
    if( sqlite3_open( homePath.c_str(), &_db ) != SQLITE_OK ) 
        throw std::runtime_error("Error while starting sqlite3");

    checkTables( _db );
}

Database::~Database() {
	if( _db )
		sqlite3_close( _db );
}

#define USER_ID 1
// in future i have to pass user id. not it just 1
void Database::insertUptimeRecord( const ProcessInfo& info ) {
	recTime_t recTime = getCurrentTime();

	insertUptimeRecord( info, recTime );
}

// i should use extended sql request to improve performance
void Database::insertUptimeRecord( const ProcessInfo& info, recTime_t time ) {
	int appId = getAppId( info );

	if( appId == -1 ) 
		appId = insertApp( info.name.c_str() );


	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2( _db, sqlInsertRecord, sizeof(sqlInsertRecord), &stmt, nullptr );

	if( rc == SQLITE_OK ) {
		rc = sqlite3_bind_int( stmt , 1, appId );
		rc += sqlite3_bind_int64( stmt , 2, time.count() );
		rc += sqlite3_bind_int( stmt , 3, info.uptime.count() );
        rc += sqlite3_bind_text( stmt, 4, info.describe.data(), info.describe.size(), SQLITE_TRANSIENT );

	} else {
		const char* zErrMsg = sqlite3_errmsg(_db);
		logger.log(LogLvl::Error, "Cannot write an record into db");
		throw std::runtime_error(zErrMsg);
	}

	rc = sqlite3_step( stmt );
	if( rc != SQLITE_DONE ) {
		const char* zErrMsg = sqlite3_errmsg(_db);
		logger.log(LogLvl::Error, "Cannot write an record into db");
		throw std::runtime_error(zErrMsg);
	}

	sqlite3_finalize( stmt );

	logger.log(LogLvl::Info, "an record inserted");
}

int Database::getAppId( const ProcessInfo& info ) {
	int toReturn = -1;

	// when i will be optimize code, i have to: merge requests and save stmt
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2( _db, sqlGetAppId, sizeof(sqlGetAppId), &stmt, nullptr );

	if( rc == SQLITE_OK ) {
		sqlite3_bind_text( stmt, 1, info.name.data(), info.name.size(), SQLITE_TRANSIENT );
		
		if( sqlite3_step(stmt) == SQLITE_ROW ) {
			toReturn = sqlite3_column_int( stmt, 0);
		}
		
		sqlite3_finalize( stmt );

	} else {
		const char* zErrMsg = sqlite3_errmsg(_db);
		logger.log( LogLvl::Error, "Cannot get app id");
		throw std::runtime_error(zErrMsg);
	}

	return toReturn;

}

int Database::insertApp( const char* appName ) {
	int appId;

	sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2( _db, sqlInsertApp, sizeof(sqlInsertApp), &stmt, nullptr );

    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, appName, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        appId = static_cast<int>(sqlite3_last_insert_rowid(_db));
        sqlite3_finalize(stmt);
    } else {
	    const char* errMsg = sqlite3_errmsg(_db);
	    logger.log( LogLvl::Error, errMsg);
	    throw std::runtime_error("Cannot insert new app");
    }
	logger.log( LogLvl::Info, "Inserted new app with id: ", appId );
	return appId;
}

// FIXME change distance to size
void Database::dumpStorage( Storage& store ) {
	logger.log(LogLvl::Info, "dumped: ", std::distance( store.begin(), store.end()));

	for( auto& record : store ) {
		insertUptimeRecord( record.info, record.recTime );
	}

	store.clear();
}

size_t Database::getRecordsCount() {
    std::size_t count{0};
	int rc;

	sqlite3_stmt* stmt;
	rc = sqlite3_prepare_v2( _db, sqlGetCount, sizeof(sqlGetCount), &stmt, nullptr );

	if( rc == SQLITE_OK ) {
		if( sqlite3_step(stmt) == SQLITE_ROW ) {
            count = sqlite3_column_int( stmt, 0);
        }

		sqlite3_finalize( stmt );

	} else {
		const char* zErrMsg = sqlite3_errmsg(_db);
		logger.log( LogLvl::Error, "Cannot get app id");
		throw std::runtime_error(zErrMsg);
	}

    return count;
}

const unsigned char* Database::getAppName( int appId ){
    int rc;
    const unsigned char* toReturn = nullptr;

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2( _db, sqlGetAppName, sizeof(sqlGetAppName), &stmt, nullptr );

    if( rc == SQLITE_OK ) {
		sqlite3_bind_int( stmt, 1, appId );
		
		if( sqlite3_step(stmt) == SQLITE_ROW ) {
			toReturn = sqlite3_column_text( stmt, 0);
		}
		
		sqlite3_finalize( stmt );

	} else {
		const char* zErrMsg = sqlite3_errmsg(_db);
		logger.log( LogLvl::Error, "cannot get app name");
		throw std::runtime_error(zErrMsg);
	}

	return toReturn;
}
