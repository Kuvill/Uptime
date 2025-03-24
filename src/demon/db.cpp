#include "inc/db.hpp"
#include "inc/ram_storage.hpp"
#include "inc/logger.hpp"

#include <sqlite3.h>

#include <stdexcept>
#include <cstring>
#include <ctime>

// does move Tables name to defines - good idea?

const char* CREATE_TABLES = "CREATE TABLE IF NOT EXISTS Users (" \
						"user_id INTEGER PRIMARY KEY," \
						"user_name TEXT"
				");" \

				"CREATE TABLE IF NOT EXISTS Applications (" \
						"app_id INTEGER PRIMARY KEY," \
						"type_id INTEGER," \
						"app_name TEXT," \
						"FOREIGN KEY(type_id) REFERENCES Categories(type_id)"
				");" \

				"CREATE TABLE IF NOT EXISTS Records (" \
									/*"rec_id INTEGER PRIMARY KEY," \ */
						"user_id INTEGER," \
						"app_id INTEGER," \
						"rec_time DATETIME," \
						"uptime INTEGER," \
						"FOREIGN KEY(user_id) REFERENCES Users(user_id)," \
						"FOREIGN KEY(app_id) REFERENCES Applications(app_id)"
				");" \

				"CREATE TABLE IF NOT EXISTS Categories (" \
						"type_id INTEGER PRIMARY KEY," \
						"type_name TEXT" \
				");";

				

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
	if( sqlite3_open( dbName, &_db ) )
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
	std::time_t recTime;
	std::time(&recTime);

	insertUptimeRecord( info, recTime );
}

void Database::insertUptimeRecord( const ProcessInfo& info, std::time_t time ) {
	int appId = getAppId( info );

	if( appId == -1 ) 
		appId = insertApp( info.name.c_str() );

	const char sql[] = "INSERT INTO Records (" \
			    "user_id, app_id, rec_time, uptime) " \
			    "VALUES (1, ?1, ?2, ?3);";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2( _db, sql, sizeof(sql), &stmt, nullptr );

	if( rc == SQLITE_OK ) {
		rc = sqlite3_bind_int( stmt , 1, appId );
		rc += sqlite3_bind_int64( stmt , 2, time );
		rc += sqlite3_bind_int( stmt , 3, info.uptime );

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

	const char getAppIdSQL[] = "SELECT app_id FROM Applications WHERE app_name = ?1;";
	// when i will be optimize code, i have to: merge requests and save stmt
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2( _db, getAppIdSQL, sizeof(getAppIdSQL), &stmt, nullptr );

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

    const char insertAppSQL[] = "INSERT INTO Applications (app_name) VALUES (?1);";
	sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2( _db, insertAppSQL, sizeof(insertAppSQL), &stmt, nullptr );

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
		insertUptimeRecord( {record.name, record.uptime}, record.recTime );
	}

	store.clear();
}
