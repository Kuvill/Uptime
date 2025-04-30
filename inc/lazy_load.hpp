#pragma once

#include <gtk/gtk.h>

#include "inc/time.hpp"
#include "inc/record_item.hpp"

// that is strut hell imo. i have to orginize them.
// never worked with gui befor, i have to change
enum class Duration {
    ByDay,
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

void on_stack_page_changed( GtkStack* stack, GParamSpec* pspec, gpointer data );

gboolean update_data( gpointer data );
