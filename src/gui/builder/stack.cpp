#include <gtk/gtk.h>

#include <common/logger.hpp>


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

static void on_stack_page_changed( GtkStack* stack, GParamSpec* pspec, gpointer data ) {
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

void setup_stack( GtkBuilder* builder ) {
    // Fill table from db
    auto* stack = GTK_STACK( gtk_builder_get_object( builder, "body" ) );
    g_signal_connect( stack, "notify::visible-child", G_CALLBACK(on_stack_page_changed), nullptr );
    on_stack_page_changed(stack, 0, nullptr);
}
