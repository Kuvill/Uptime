#pragma once

#include "demon/settings.hpp"
#include "gui/record_item.hpp"
#include "gui/db.hpp"
#include "gui/client.hpp"
#include "common/time.hpp"

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

// It should be init once at start and then on Bound changings
class State {
    // sorted
    GListStore* _store;
    Bound _bound;

    GTimer* _timer;

    // should be just guint64, but i don't query it from db
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


