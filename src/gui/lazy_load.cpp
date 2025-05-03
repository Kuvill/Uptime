#include <gio/gio.h>

#include "inc/logger.hpp"
#include "inc/time.hpp"
#include "inc/lazy_load.hpp"
#include "inc/record_item.hpp"

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
