#include "gio/gio.h"
#include "gui/context.hpp"
#include "common/logger.hpp"

#include "gtk/gtk.h"

static void test( GSimpleAction* action, GVariant* param, gpointer data ) {
    logger.log(LogLvl::Error, "Works well!");
}

static void on_year_duration_choose( GSimpleAction* action, GVariant* param, gpointer data ) {
    Context::get()->state.changeModel( Duration::ByYear );
}

static void on_month_duration_choose( GSimpleAction* action, GVariant* param, gpointer data ) {
    Context::get()->state.changeModel( Duration::ByYear );
}
static void on_day_duration_choose( GSimpleAction* action, GVariant* param, gpointer data ) { Context::get()->state.changeModel( Duration::ByYear );
}
static void on_all_duration_choose( GSimpleAction* action, GVariant* param, gpointer data ) {
    Context::get()->state.changeModel( Duration::ByYear );
}

static GActionEntry app_entries[] = {
    {"years", test, nullptr, nullptr, nullptr},
    {"months", test, nullptr, nullptr, nullptr},
    {"days", test, nullptr, nullptr, nullptr},
    {"all", test, nullptr, nullptr, nullptr},
};

void setup_table_navigation( GtkApplication* app, GtkBuilder* builder ) {

    // works well from inspector. now need button support
    g_action_map_add_action_entries( G_ACTION_MAP( app ), 
                                    app_entries, G_N_ELEMENTS(app_entries), nullptr);

    auto* btn = gtk_builder_get_object( builder, "DurationBtn" );
    GTK_POPOVER(gtk_menu_button_get_popover( GTK_MENU_BUTTON(btn) ));
}
