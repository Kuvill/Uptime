#pragma once

// lot of unused symbols, but guess this variant more readable
#include "gui/record_item.hpp"
#include "common/time.hpp"

#include <gtk/gtk.h>
#include <sqlite3.h>
#include <type_traits>

enum class Operators {
    Grate,
    GrateOrEqaul,
    Less,
    LessOrEqaul,
    Eqal
};

// I have to overload constructor to open read only
// + mb i have to store GListStore* here

// There is no virtual destructor.
class DatabaseReader {
protected:
    sqlite3* _db;

public:
    DatabaseReader( const char* dbName );
    DatabaseReader( const char* dbName, std::true_type );
    ~DatabaseReader();

    std::tuple<RecordItem**, int> getRecords( Operators op, recTime_t );
    RecordItem* getLastRecord();
    const unsigned char* getAppName( int appId );
    void checkErr( const char* = "" );
};
