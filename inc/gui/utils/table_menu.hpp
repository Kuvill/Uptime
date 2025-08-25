#pragma once

#include <gtk/gtk.h>

struct TableMenu {
    GtkPopover* popover;

    TableMenu() {
        auto* builder = gtk_builder_new_from_file( "table_menu.ui" );

        popover = GTK_POPOVER( gtk_builder_get_object( builder, "table_menu" ) );

        g_object_unref( builder );
    }

    ~TableMenu() {
        g_object_unref( popover );
    }
};
