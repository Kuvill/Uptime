#pragma once

// lot of unused symbols, but guess this variant more readable
#include "db.hpp"
#include "record_item.hpp"

#include <gtk/gtk.h>

enum class Operators {
    Grate,
    GrateOrEqaul,
    Less,
    LessOrEqaul,
    Eqal
};

// There is no virtual destructor.
class DatabaseReader : Database {
public:
    GListStore* getRecords( Operators op, recTime_t );
};
