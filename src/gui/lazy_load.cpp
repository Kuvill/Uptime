#include <gio/gio.h>

#include "inc/logger.hpp"
#include "inc/time.hpp"
#include "inc/lazy_load.hpp"
#include "inc/record_item.hpp"

using namespace std::chrono;

// OOP way: create state pattern class, that remember state.
// more cheap way: create static function getDif
    void State::changeModel( Duration ) {
        logger.log(LogLvl::Info, "Changing view model");

        // looks really ugly
        // only way that i see - use variant over enum 
        // performance the same ( 4-8 byte loose. but i have only once instance)
        // but will it appearance?
        switch (_bound.model) {
            case Duration::ByYear:
                _bound.from = duration_cast<years>( getCurrentTime() );
                _bound.to = _bound.from + years(1);
                break;

            case Duration::ByMonth:
                _bound.from = duration_cast<months>( getCurrentTime() );
                _bound.to = _bound.from + months(1);
                break;

            case Duration::ByDay: _bound.from = duration_cast<days>( getCurrentTime() ); _bound.to = _bound.from + days(1);
                break;
                
            case Duration::All:
                _bound.from = recTime_t::min();
                _bound.to = recTime_t::max();
                break;
        }
    }

    void State::next() {
        switch (_bound.model) {
            case Duration::ByYear:
                _bound.to = _bound.from + years(1);
                break;

            case Duration::ByMonth:
                _bound.to = _bound.from + months(1);
                break;

            case Duration::ByDay:
                _bound.to = _bound.from + days(1);
                break;
                
            case Duration::All:
                break;
        }
    }
    void State::prev() {
        switch (_bound.model) {
            case Duration::ByYear:
                _bound.to = _bound.from - years(1);
                break;

            case Duration::ByMonth:
                _bound.to = _bound.from - months(1);
                break;

            case Duration::ByDay:
                _bound.to = _bound.from - days(1);
                break;
                
            case Duration::All:
                break;
        }
    }

    void State::setFrom() {

    }
    void State::setTo() {

    }

    void State::mergeStore( RecordItem** items ) {
        g_list_store_splice(_store, 0, 0, (void**)(items), 10);

        // here i delete 2 pointers. Not items
        delete items;
    }

    void State::setStore( GListStore* store ) {
        _store = store;
    }

    GListStore* State::getStore() { return _store; }

static const gchar* TABLE_PAGE = "table";
static const gchar* CHARTS_PAGE = "charts";
[[maybe_unused]] static const gchar* SETTINGS_PAGE = "settings";
[[maybe_unused]] static const gchar* ABOUT_PAGE = "about";

static void load_table() {
    logger.log(LogLvl::Error, "Load table not working yet");
}
static void load_charts() {
    logger.log(LogLvl::Error, "Load charts not working yet");
}

void on_stack_page_changed( GtkStack* stack, GParamSpec* pspec, gpointer data ) {
    logger.log(LogLvl::Info, "Stack page switched" );
    const gchar* newPage = gtk_stack_get_visible_child_name( stack );

    if( g_strcmp0( newPage, TABLE_PAGE ) == 0 ) {
        load_table();

    } else if( g_strcmp0( newPage, CHARTS_PAGE ) == 0 ) {
        load_charts();

    } else if( g_strcmp0( newPage, SETTINGS_PAGE ) == 0 ) {

    } else if( g_strcmp0( newPage, ABOUT_PAGE ) == 0 ) {

    } else {
        logger.log(LogLvl::Error, "Unprocessing Stack Page"); 
    }
}

gboolean update_data( gpointer data ) {
    logger.log(LogLvl::Error, "Timer not impl yet" );

    return G_SOURCE_CONTINUE;
}
