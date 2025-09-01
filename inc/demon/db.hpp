#pragma once

#include "demon/get_uptime.hpp"
#include "demon/ram_storage.hpp"
#include "common/time.hpp"

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
    void insertManyUptimeRecords( std::string_view );
    
protected:
    sqlite3* _db;

    Database();

public:
	Database( const char* dbName );
	~Database();

	bool insertUser( const char* userName );

	// upon two mehod are useless?
	void dumpStorage( Storage& );
	void insertUptimeRecord( const ProcessInfo& );

	size_t getRecordsCount();
    const unsigned char* getAppName( int appId );
    Storage getRecords();
};
