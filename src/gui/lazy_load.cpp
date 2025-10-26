#include <print>

#include "gtk/gtk.h"
#include "common/logger.hpp"
#include "gui/lazy_load.hpp"
#include "gui/context.hpp"
#include "gui/record_item.hpp"

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
            updateUptime( newItem, item, std::chrono::seconds(5) );
            logger.log(LogLvl::Info, "Replace record info: ", newItem->appName, ", ", newItem->uptime );
            // g_object_set( item, "name", newItem->appName, "uptime", newItem->uptime, nullptr );

    } else {
        logger.log( LogLvl::Info, "New record added" );
        g_list_store_insert_sorted( store, newItem, RecordItemNameCompare, nullptr );
    }

    context.state.setLastItem( newItem );

    return G_SOURCE_CONTINUE;
}
