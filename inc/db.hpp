#include "inc/get_uptime.hpp"
#include "inc/ram_storage.hpp"

#include <sqlite3.h>
#include <cstdint>

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
	sqlite3* _db;

	int insertApp(const char* appName );
	int getAppId( const ProcessInfo& info );

public:
	
	Database( const char* dbName );
	~Database();

	bool insertUser( const char* userName );
	
	void insertUptimeRecord( uint32_t userId, uint32_t appId, uint64_t uptime, uint64_t date);

	// upon two mehod are useless?
	void dumpStorage( const Storage& );
	void insertUptimeRecord( const ProcessInfo& );
};
