#pragma once

// lot of unused symbols, but guess this variant more readable
#include "db.hpp"
#include "columnView.hpp"

#include <gtk/gtk.h>

enum class Operators {
    Grate,
    Less,
    Eqal
};

// There is no virtual destructor.
class DatabaseReader : Database {
public:
    GListStore* getRecords( Operators op, std::time_t );
};
