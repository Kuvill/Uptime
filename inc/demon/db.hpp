#pragma once

#include "common/logger.hpp"
#include "common/time.hpp"
#include "demon/process_info.hpp"

#include <exception>
#include <ranges>
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
	void insertUptimeRecord( const ProcessInfo& );

	size_t getRecordsCount();
    const unsigned char* getAppName( int appId );
    // implement range based? - ideal solutio
    template< std::ranges::range Range >
    Range getRecords();

    template< std::ranges::range Range, typename Proj >
        // require values in range to be ProcessInfo
        requires std::same_as<std::remove_cvref_t<std::invoke_result_t<Proj, std::ranges::range_value_t<Range>>>, ProcessInfo>
    void insertFrom( Range&& range, Proj proj = Proj() );
};

template< std::ranges::range Range >
Range Database::getRecords() {
    static_assert(false, "didn't implemented yet");
}

template< std::ranges::range Range, typename Proj >
    requires std::same_as<std::remove_cvref_t<std::invoke_result_t<Proj, std::ranges::range_value_t<Range>>>, ProcessInfo>

void Database::insertFrom( Range&& range, Proj proj ) {
    try {
        sqlite3_exec( _db, "BEGIN;", nullptr, nullptr, nullptr );

        for( auto& record : range ) {
            insertUptimeRecord( record.info );
        }

        sqlite3_exec( _db, "COMMIT;", nullptr, nullptr, nullptr );
    }

    catch( ... ) {
        logger.log(LogLvl::Error, "Exception dropped while inserting and range into db!");
        std::rethrow_exception( std::current_exception() );
    }
}
