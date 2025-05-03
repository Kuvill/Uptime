#include "glib.h"
#include "inc/logger.hpp"
#include "inc/record_item.hpp"
#include "inc/time.hpp"
#include "inc/db_out.hpp"

#include <gtk/gtk.h>
#include <sqlite3.h>
#include <cmath>
#include <stdexcept>
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
static const char sqlGetRecords[] = "SELECT app_name, uptime, datetime(rec_time, 'unixepoch') AS rec_date " \
                                    "FROM " REC_TABLE " JOIN " APP_TABLE " ON " REC_TABLE ".app_id = " APP_TABLE ".app_id " \
                                    "WHERE rec_date > datetime(?1, 'unixepoch');";

static const char sqlGetAppName[] = "SELECT app_name FROM " APP_TABLE " WHERE app_id = ?1;";

static const char sqlGetLastRecord[] = "SELECT app_name, uptime, datetime(rec_time, 'unixepoch') AS rec_date " \
                                        "FROM " REC_TABLE " JOIN " APP_TABLE " ON " REC_TABLE ".app_id = " APP_TABLE ".app_id " \
                                        "ORDER BY rec_time LIMIT 1;";




static int countRows( sqlite3_stmt *stmt ) {
    int count = 0;
    while (sqlite3_step( stmt ) == SQLITE_ROW)
        count++;

    sqlite3_reset( stmt );
    return count;
}



DatabaseReader::DatabaseReader( const char* dbName ) {
    if( sqlite3_open_v2( dbName, &_db, SQLITE_OPEN_READONLY, nullptr ) != SQLITE_OK ) {
		const char* zErrMsg = sqlite3_errmsg(_db);
		throw std::runtime_error(zErrMsg);
    }
}

DatabaseReader::DatabaseReader( const char* dbName, std::true_type ) {
    std::string homePath = std::getenv( "HOME" );
    homePath.push_back('/');
    homePath.append( dbName );

    logger.log(LogLvl::Info, "DB path: ", homePath );
    if( sqlite3_open_v2( homePath.c_str(), &_db, SQLITE_OPEN_READONLY, nullptr ) != SQLITE_OK ) {
        throw std::runtime_error("Error while opening sqlite3");
    }

}

DatabaseReader::~DatabaseReader() {
	if( _db )
		sqlite3_close( _db );
}

// FIXME check performance with vector
// @you have to delete returned value;
std::tuple<RecordItem**, int> DatabaseReader::getRecords( Operators op, recTime_t time ) {
	RecordItem** items;
    int count;

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2( _db, sqlGetRecords, sizeof(sqlGetRecords), &stmt, nullptr );

	if( rc == SQLITE_OK ) {
        rc = sqlite3_bind_int64(stmt, 1, time.count());
        if( rc != SQLITE_OK ) throw std::runtime_error("err");

        count = countRows( stmt );
        // count = 10;
        logger.log(LogLvl::Info, "loaded ", count, " record" );

        items = new RecordItem*[ count ];

		for( int i = 0; i < count; ++i ) {
            if( sqlite3_step( stmt ) != SQLITE_ROW ) throw std::runtime_error("13123");
            RecordItem* item = record_item_new();

            // auto a = getAppName( sqlite3_column_int(stmt, 0) );
            auto appName = (sqlite3_column_text( stmt, 0 ));

            // i don't want to call notify. Guess this is misstake
            item->appName = g_strdup( (gchar*)appName );
            item->uptime = toRecTime( sqlite3_column_int(stmt, 1) );
            // rec.recTime = sqlite3_column_int(stmt, 2);

            items[i] = item;
		}

        sqlite3_finalize( stmt );

	} else {
		const char* zErrMsg = sqlite3_errmsg(_db);
		throw std::runtime_error(zErrMsg);
	}

    return {items, count};
}

RecordItem* DatabaseReader::getLastRecord() {
    RecordItem* item = record_item_new();

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
        logger.log(LogLvl::Warning, "Required add id: ", appId );

		if( sqlite3_step(stmt) == SQLITE_ROW ) {
            logger.log(LogLvl::Warning, "App name have to be getted");
			toReturn = sqlite3_column_text( stmt, 0);
		} else {
            const char* zErrMsg = sqlite3_errmsg( _db );
            logger.log(LogLvl::Warning, "not row");
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
        logger.log(LogLvl::Warning, "Steel the same");

    return toReturn;
}
