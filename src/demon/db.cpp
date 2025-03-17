#include "inc/db.hpp"

#include <sqlite3.h>
#include <stdexcept>

const char* CREATE_TABLES = "CREATE TABLE IF NOT EXISTS Users (" \
							"user_id INTEGER PRIMARY KEY," \
							"user_name TEXT);" \

							"CREATE TABLE IF NOT EXISTS Applications (" \
							"app_id INTEGER PRIMARY KEY," \
							"app_name TEXT);" \

							"CREATE TABLE IF NOT EXISTS Records (" \
									"rec_id INTEGER PRIMARY KEY," \
									"user_id INTEGER," \
									"app_id INTEGER," \
									"rec_time DATETIME," \
									"uptime INTEGER," \
									"FOREIGN KEY(user_id) REFERENCES Users(user_id)," \
									"FOREIGN KEY(app_id) REFERENCES Applications(app_id) );";

// create tables if not
void checkTables( sqlite3* db ) {
	char* zErrMsg = nullptr;
	int rc;

	rc = sqlite3_exec( db, CREATE_TABLES, nullptr, 0, &zErrMsg );

	if( rc != SQLITE_OK ) 
		throw std::runtime_error(zErrMsg);
}

Database::Database( const char* dbName ) {
	if( sqlite3_open( dbName, &_db ) )
		throw std::runtime_error("Error while starting sqlite3");

	checkTables( _db );
}

Database::~Database() {
	if( _db )
		sqlite3_close_v2( _db );
}
