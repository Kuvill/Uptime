#include <gio/gio.h>
#include <print>

#include "glib-object.h"
#include "gtk/gtk.h"
#include "inc/logger.hpp"
#include "inc/lazy_load.hpp"
#include "inc/context.hpp"
#include "record_item.hpp"

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
    if( item->appName == newItem->appName ) {
    // difference instance (1. mb i should add epsilon.)
        if( abs(item->uptime - newItem->uptime) > cd )
            // newItem->uptime += item->uptime;
            g_object_set( newItem, "kek;", 123, nullptr );

        // sequence record. Btw 1 tact loose. i can save abs result and use it in max
        else
            // newItem->uptime = std::max(newItem->uptime, item->uptime);
            // g_object_set( newItem, newItem->appName, std::max(newItem->uptime, item->uptime), nullptr );
            g_object_set( newItem, "kekw", 123, nullptr );
    }

}


// to implement expanded view, i have to release my own list moddel
// coz the data should be applyable to both types of graphic.
// + better filter implement + more easyest support at general
gboolean update_data( gpointer data ) {
    logger.log(LogLvl::Error, "Timer not impl yet" );

    auto& context = *static_cast<Context*>(data);
    auto* store = context.state.getStore();
    auto* newItem = context.db.getLastRecord();

    // from this: check how to merge record

    // use find by name. There is <= 1 item with same name
    int index = g_list_store_insert_sorted(context.state.getStore(), newItem, RecordItemNameCompare, nullptr );
    logger.log(LogLvl::Info, index );


    // g_list_model_get_n_items - not working :D
    guint count = 0;
    RecordItem* temp = RECORD_ITEM(g_list_model_get_item( G_LIST_MODEL(store), count ));
    while( temp != nullptr ) {
        ++count;
        temp = RECORD_ITEM(g_list_model_get_item( G_LIST_MODEL(store), count ));
    }

    // can make it shorter
    if( index < count - 1 ) {
        logger.log(LogLvl::Info, "index+1" );
        auto* item = (RECORD_ITEM(g_list_model_get_item( G_LIST_MODEL(store), index+1 )));
        if( item->appName == newItem->appName ) {
            updateUptime( newItem, item, context.settings.cd );
            g_list_store_remove(store, index+1);
        }

    } if( index > 0 ) {
        logger.log(LogLvl::Info, "index-1" );
        auto* item = (RECORD_ITEM(g_list_model_get_item( G_LIST_MODEL(store), index-1 )));
        if( item->appName == newItem->appName ) {
            updateUptime( newItem, item, context.settings.cd );
            g_list_store_remove(store, index-1);
        }
    }

    // FIXME
    return G_SOURCE_CONTINUE;
}

void onDurationAction( GSimpleAction* self, GVariant* param, gpointer data ) { 
}
