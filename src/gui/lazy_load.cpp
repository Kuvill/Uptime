#include <print>

#include "gtk/gtk.h"
#include "common/logger.hpp"
#include "gui/lazy_load.hpp"
#include "gui/context.hpp"
#include "gui/record_item.hpp"

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

    Context& context = *static_cast<Context*>(data);
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

// FIXME
// btw i think i have somethings like this in ?db_out?
static void updateUptime( RecordItem* newItem, RecordItem* item, std::chrono::seconds cd ) {
    // difference instance (1. mb i should add epsilon.)
    if( abs(item->uptime - newItem->uptime) > cd ) {
        auto newUptime = newItem->uptime + item->uptime;
        g_object_set( item, "uptime", newUptime, nullptr );

    // sequence record. Btw 1 tact loose. i can save abs result and use it in max
    } else {
       // newItem->uptime = std::max(newItem->uptime, item->uptime);
       // g_object_set( newItem, newItem->appName, std::max(newItem->uptime, item->uptime), nullptr );
       auto newUptime = std::max(newItem->uptime, item->uptime);
       g_object_set( item, "uptime", newUptime, nullptr );
    }
}


// to implement expanded view, i have to release my own list moddel
// coz the data should be applyable to both types of graphic.
// + better filter implement + more easyest support at general
gboolean update_data( gpointer data ) {
    auto& context = *static_cast<Context*>(data);
    auto* store = context.state.getStore();
    auto* newItem = context.db.getLastRecord();

    // should be better way
    auto lastItem = context.state.getLastItem();

    if( lastItem != nullptr && RecordItemEqual( lastItem, newItem ) ) {
        logger.log(LogLvl::Warning, "Queried item is same as preview");
        return G_SOURCE_CONTINUE;
    }

    // from this: check how to merge record

    guint index;
    bool isOverlap = g_list_store_find_with_equal_func
            (store, (void*)newItem, RecordItemNameEqual, &index);

    // there is max 1 item with same name
    if( isOverlap ) {
        logger.log(LogLvl::Info, "Merging records...");
            auto* item = (RECORD_ITEM(g_list_model_get_item( G_LIST_MODEL(store), index )));
            updateUptime( newItem, item, context.settings.cd );
            logger.log(LogLvl::Info, "Replace record info: ", newItem->appName, ", ", newItem->uptime );
            // g_object_set( item, "name", newItem->appName, "uptime", newItem->uptime, nullptr );

    } else {
        logger.log( LogLvl::Info, "New record added" );
        g_list_store_insert_sorted( store, newItem, RecordItemNameCompare, nullptr );
    }

    context.state.setLastItem( newItem );

    return G_SOURCE_CONTINUE;
}
