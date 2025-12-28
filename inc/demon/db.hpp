#pragma once

#include "common/time.hpp"
#include "demon/process_info.hpp"

#include <iterator>
#include <ranges>
#include <sqlite3.h>
#include <type_traits>


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

    auto dumpStorage();
	void insertUptimeRecord( const ProcessInfo& );

	size_t getRecordsCount();
    const unsigned char* getAppName( int appId );
    // Storage getRecords();
    template<template<typename...> typename Container>
    auto getRecords();

    template< std::ranges::range Range >
    Range insertFrom();
};

template<template<typename...> typename Container>
auto getRecords() {
    static_assert(false, "didn't implemented yet");
}

template< std::ranges::range Range >
Range Database::insertFrom() {
    static_assert(false);
}
