#include <inc/db_out.hpp>

#define USERS_TABLE "Users"
#define APP_TABLE "Applications"
#define REC_TABLE "Records"
#define CATEGOR_TABLE "Categories"

static const char sqlGetRecords[] = "SLECT * FROM " REC_TABLE \
                                     "WHERE ";

GListStore* DatabaseReader::getRecords( Operators op, std::time_t ) {
	Storage toReturn;

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2( _db, sqlGetRecords, sizeof(sqlGetRecords), &stmt, nullptr );

    // FIXME potential out-of-bound
	if( rc == SQLITE_OK ) {
		while( sqlite3_step(stmt) == SQLITE_ROW ) {
            Record rec;

            rec.name = getAppName( sqlite3_column_int(stmt, 1) );
            rec.uptime = sqlite3_column_int(stmt, 2);
            rec.recTime = sqlite3_column_int(stmt, 3);

            toReturn.insert( {rec.name, rec.uptime}, rec.recTime );
		}
		
		sqlite3_finalize( stmt );

	} else {
		const char* zErrMsg = sqlite3_errmsg(_db);
		logger.log( LogLvl::Error, "Cannot get app id");
		throw std::runtime_error(zErrMsg);
	}

    return toReturn;
}

