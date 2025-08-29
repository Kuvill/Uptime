#include "gio/gio.h"
#include "gui/context.hpp"

#include "gtk/gtk.h"

// Make one function and pass data with variant?

static void on_year_duration_choose( GSimpleAction* action, GVariant* param, gpointer data ) {
    Context::get()->state.changeModel( Duration::ByYear );

    g_list_store_remove_all( Context::get()->state.getStore() );
    Context::get()->state.mergeStoreRightVersion( Context::get()->db.getRecords() );
}

static void on_month_duration_choose( GSimpleAction* action, GVariant* param, gpointer data ) {
    Context::get()->state.changeModel( Duration::ByMonth );

    g_list_store_remove_all( Context::get()->state.getStore() );
    Context::get()->state.mergeStoreRightVersion( Context::get()->db.getRecords() );
}
static void on_day_duration_choose( GSimpleAction* action, GVariant* param, gpointer data ) { Context::get()->state.changeModel( Duration::ByYear );
    Context::get()->state.changeModel( Duration::ByDay );

    g_list_store_remove_all( Context::get()->state.getStore() );
    Context::get()->state.mergeStoreRightVersion( Context::get()->db.getRecords() );
}
static void on_all_duration_choose( GSimpleAction* action, GVariant* param, gpointer data ) {
    Context::get()->state.changeModel( Duration::All );

    g_list_store_remove_all( Context::get()->state.getStore() );
    Context::get()->state.mergeStoreRightVersion( Context::get()->db.getRecords() );
}

static GActionEntry app_entries[] = {
    {"years", on_year_duration_choose, nullptr, nullptr, nullptr},
    {"months", on_month_duration_choose, nullptr, nullptr, nullptr},
    {"days", on_day_duration_choose, nullptr, nullptr, nullptr},
    {"all", on_all_duration_choose, nullptr, nullptr, nullptr},
};

void setup_table_navigation( GtkApplication* app, GtkBuilder* builder ) {

    // works well from inspector. now need button support
    g_action_map_add_action_entries( G_ACTION_MAP( app ), 
                                    app_entries, G_N_ELEMENTS(app_entries), nullptr);

    auto* btn = gtk_builder_get_object( builder, "DurationBtn" );
    GTK_POPOVER(gtk_menu_button_get_popover( GTK_MENU_BUTTON(btn) ));
}
