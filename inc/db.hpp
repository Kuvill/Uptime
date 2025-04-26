#pragma once

#include "inc/get_uptime.hpp"
#include "inc/ram_storage.hpp"
#include "inc/time.hpp"

#include <sqlite3.h>


// 2 variants:

// 1 table with all data (with mixed accounts), and other with all users

// such tables as users
// Irina says, that more tables (with bounds, ofc) - better hashing - better time
// Create table with accs, apps, and third uptimes.


// I want to get have:
// user stats for a day / all data.
// by morning/halfday/night
// by all users


class Database {
	int insertApp(const char* appName );
	int getAppId( const ProcessInfo& info );
	void insertUptimeRecord( const ProcessInfo&, recTime_t );
    
protected:
    sqlite3* _db;

public:
	Database( const char* dbName );
	~Database();

	bool insertUser( const char* userName );

	// upon two mehod are useless?
	void dumpStorage( Storage& );
	void insertUptimeRecord( const ProcessInfo& );

	size_t getRecordsCount();
    const char* getAppName( int appId );
    Storage getRecords();
};
