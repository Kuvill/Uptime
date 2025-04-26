#include "gio/gio.h"
#include "inc/logger.hpp"
#include "inc/record_item.hpp"
#include "inc/time.hpp"
#include "inc/db_out.hpp"

#include <gtk/gtk.h>

#define USERS_TABLE "Users"
#define APP_TABLE "Applications"
#define REC_TABLE "Records"
#define CATEGOR_TABLE "Categories"

static const char sqlGetRecords[] = "SLECT * FROM " REC_TABLE \
                                     "WHERE ";

GListStore* DatabaseReader::getRecords( Operators op, recTime_t time ) {
	GListStore* store = g_list_store_new( RECORD_ITEM_TYPE );

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2( _db, sqlGetRecords, sizeof(sqlGetRecords), &stmt, nullptr );

    // FIXME potential out-of-bound
	if( rc == SQLITE_OK ) {
		while( sqlite3_step(stmt) == SQLITE_ROW ) {
            RecordItem* item = record_item_new();

            // i don't want to call notify. Guess this is misstake
            item->appName = g_strdup( getAppName( sqlite3_column_int(stmt, 1) ) );
            item->uptime = sqlite3_column_int(stmt, 2);
            // rec.recTime = sqlite3_column_int(stmt, 3);

            g_list_store_append( store, item );
            g_object_unref( item );
		}
		
		sqlite3_finalize( stmt );

	} else {
		const char* zErrMsg = sqlite3_errmsg(_db);
		logger.log( LogLvl::Error, "Cannot get app id");
		throw std::runtime_error(zErrMsg);
	}

    return store;
}
