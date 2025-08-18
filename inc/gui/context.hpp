#pragma once

#include "demon/settings.hpp"
#include "gui/record_item.hpp"
#include "gui/db.hpp"
#include "gui/client.hpp"
#include "common/time.hpp"

#include <gtk/gtk.h>

enum class Duration { 
    ByDay,
    ByMonth,
    ByYear,
    All
};

struct Bound {
    std::chrono::year_month_day to = getCurrentDate();
    std::chrono::year_month_day from = getCurrentDate();
    
    Duration model = Duration::ByDay;

    Bound();
};

class State {
    GListStore* _store;
    Bound _bound;

    GTimer* _timer;

    RecordItem* _lastItem = nullptr;

public:
    RecordItem* getLastItem() { return _lastItem; }
    void setLastItem( RecordItem* item ) { _lastItem = item; }

    void createTimer();

    // fucking slave
    void mergeStoreRightVersion( std::tuple<RawRecordItem**, int> );

    void mergeStore( std::tuple<RawRecordItem**, int> );
    void mergeStoreUnique(  std::tuple<RawRecordItem**, int> );
    void setStore( GListStore* );
    GListStore* getStore();

    // --- Bound

    void changeModel( Duration );

    void next();
    void prev();

    void setFrom();
    void setTo();

    friend struct Context;
};

struct Context {
    DatabaseReader db;
    Client ipc;
    State state;
    Settings settings;
};


