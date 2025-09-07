#pragma once

#include <common/time.hpp>
#include <gui/record_item.hpp>

#include <gtk/gtk.h>

enum class Duration { 
    ByDay,
    ByMonth,
    ByYear,
    All
};

struct Bound {
    // i want to use year_month_day,
    // but guess sqlite can make better optimizations, that just 2 bite save
    recTime_t to = getCurrentTime();
    recTime_t from = getCurrentTime();
    
    Duration model = Duration::All;

    Bound();
};

class State {
    GListStore* _store;

    GTimer* _timer;

    RecordItem* _lastItem = nullptr;

public:
    Bound _bound;

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

    friend class Context;
};


