#include "common/logger.hpp"
#include "gui/record_item.hpp"
#include "common/time.hpp"
#include "common/change_dir.hpp"
#include "gui/db.hpp"

#include <gtk/gtk.h>
#include <sqlite3.h>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <type_traits>

#define USERS_TABLE "Users"
#define APP_TABLE "Applications"
#define REC_TABLE "Records"
#define CATEGOR_TABLE "Categories"

/* Just let it here
   SELECT app_name, uptime, datetime(rec_time, 'unixepoch') AS rec_date
FROM Records JOIN Applications ON Records.app_id  = Applications.app_id
WHERE rec_date > datetime('2025-04-26 08:00:00');
*/

// to get last record, i can Order by rec_time with LIMIT 1
// (BEFOR I HAVE to check, is there spetific function (mb with WAL))
static const char sqlGetRecords[] = "SELECT app_name, rec_time AS rec_date " \
                                    "FROM " REC_TABLE " JOIN " APP_TABLE " ON " REC_TABLE ".app_id = " APP_TABLE ".app_id " \
                                    "WHERE datetime(rec_date, 'unixepoch') > datetime(?1, 'unixepoch');";

static const char sqlGetAppName[] = "SELECT app_name FROM " APP_TABLE " WHERE app_id = ?1;";

static const char sqlGetLastRecord[] = "SELECT app_name, uptime, datetime(rec_time, 'unixepoch') AS rec_date " \
                                        "FROM " REC_TABLE " JOIN " APP_TABLE " ON " REC_TABLE ".app_id = " APP_TABLE ".app_id " \
                                        "ORDER BY rec_time LIMIT 1;";




// O(n)
static int countRows( sqlite3_stmt *stmt ) {
    int count = 0;
    while (sqlite3_step( stmt ) == SQLITE_ROW)
        count++;

    sqlite3_reset( stmt );
    return count;
}

DatabaseReader::DatabaseReader() {
    CheckDirectory();
}

DatabaseReader::DatabaseReader( const char* dbName ) : DatabaseReader() {
    if( sqlite3_open_v2( dbName, &_db, SQLITE_OPEN_READONLY, nullptr ) != SQLITE_OK ) {
		const char* zErrMsg = sqlite3_errmsg(_db);
		throw std::runtime_error(zErrMsg);
    }
}

DatabaseReader::~DatabaseReader() {
	if( _db )
		sqlite3_close( _db );
}

// @yru have to delete returned value;
// btw all param i should take from context
std::tuple<RawRecordItem**, int> DatabaseReader::getRecords( Operators op, recTime_t time ) {
    logger.log(LogLvl::Info, "Trying to get all records...");
	RawRecordItem** items;
    int count;

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2( _db, sqlGetRecords, sizeof(sqlGetRecords), &stmt, nullptr );

	if( rc == SQLITE_OK ) {
        rc = sqlite3_bind_int64(stmt, 1, time.count());
        if( rc != SQLITE_OK ) throw std::runtime_error("err");

        count = countRows( stmt );
        // count = 200;
        logger.log(LogLvl::Info, "loaded ", count, " record" );

        items = new RawRecordItem*[ count ];

		//--for( int i = 0; i < count; ++i ) {
            //-- if( sqlite3_step( stmt ) != SQLITE_ROW ) throw std::runtime_error("unexpected number of rows");

        int i = 0;
        for(; sqlite3_step( stmt ) == SQLITE_ROW; ++i ) {
            RawRecordItem* item = raw_record_item_new();

            // auto a = getAppName( sqlite3_column_int(stmt, 0) );
            auto appName = (sqlite3_column_text( stmt, 0 ));

            // i don't want to call notify. Guess this is misstake
            item->appName = g_strdup( (gchar*)appName );
            item->recTimer = toRecTime( sqlite3_column_int(stmt, 1) );

            items[i] = item;
		}
        // count = i;

        sqlite3_finalize( stmt );

	} else {
		const char* zErrMsg = sqlite3_errmsg(_db);
		throw std::runtime_error(zErrMsg);
	}

    auto err = sqlite3_errcode( _db );
    if( err != SQLITE_OK ) {
        logger.log(LogLvl::Error, "eror after all records", err );
        exit(1);
    }

    // checkErr( "end of getREcords");
    return {items, count};
}

RecordItem* DatabaseReader::getLastRecord() {
    logger.log(LogLvl::Info, "Trying to get last record...");
    RecordItem* item = record_item_new();
    // checkErr("in start of Last");

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2( _db, sqlGetLastRecord, sizeof(sqlGetLastRecord), &stmt, nullptr );

    if( rc == SQLITE_OK ) {
        if( sqlite3_step( stmt ) == SQLITE_ROW ) {
            auto appName = (sqlite3_column_text( stmt, 0 ));

            item->appName = g_strdup( (gchar*)appName );
            item->uptime = toRecTime( sqlite3_column_int( stmt, 1 ) );
        }

        sqlite3_finalize( stmt );
    } else {
        const char* zErrMsg = sqlite3_errmsg(_db);
		throw std::runtime_error(zErrMsg);
    }

    return item;

}

const unsigned char* DatabaseReader::getAppName( int appId ){
    int rc;
    const unsigned char* toReturn = nullptr;

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2( _db, sqlGetAppName, sizeof(sqlGetAppName), &stmt, nullptr );

    if( rc == SQLITE_OK ) {
		sqlite3_bind_int( stmt, 1, appId );

		if( sqlite3_step(stmt) == SQLITE_ROW ) {
			toReturn = sqlite3_column_text( stmt, 0);
		} else {
            const char* zErrMsg = sqlite3_errmsg( _db );
            logger.log(LogLvl::Error, "get App Name sql returned not a row");
            throw std::runtime_error(zErrMsg);
        }

		sqlite3_finalize( stmt );

	} else {
        sqlite3_finalize( stmt );
		const char* zErrMsg = sqlite3_errmsg(_db);
		logger.log( LogLvl::Error, "cannot get app name");
		throw std::runtime_error(zErrMsg);
	}

    if( toReturn == nullptr )
        logger.log(LogLvl::Warning, "Still the same");

    auto err = sqlite3_errcode( _db );
    if( err != SQLITE_OK ) {
        logger.log(LogLvl::Error, "eror after all records", err );
        exit(1);
    }

    return toReturn;
}

void DatabaseReader::checkErr( const char* msg ) {
    auto err = sqlite3_errcode( _db );

    if( err != SQLITE_OK )
        logger.log(LogLvl::Error, "Error with sqlite3: ", err );
}
