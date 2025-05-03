#pragma once

#include "inc/time.hpp"
#include "inc/record_item.hpp"
#include "inc/db_out.hpp"

#include <gtk/gtk.h>

enum class Duration { ByDay,
    ByMonth,
    ByYear,
    All
};

struct Bound {
    // mb should be yaer_month_day, idk
    recTime_t from = getCurrentTime();
    recTime_t to = from + std::chrono::days(1);
    Duration model = Duration::ByDay;

};

class State {
    GListStore* _store;
    Bound _bound;

public:
    void mergeStore( std::tuple<RecordItem**, int> );
    void setStore( GListStore* );
    GListStore* getStore();

    void changeModel( Duration );

    void next();
    void prev();

    void setFrom();
    void setTo();
};

struct Settings {
    std::chrono::seconds cd;
};

struct Context {
    DatabaseReader db;
    State state;
    Settings settings;
};

