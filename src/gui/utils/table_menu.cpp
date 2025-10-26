#include "gui/utils/table_menu.hpp"
#include "common/change_dir.hpp"
#include "common/logger.hpp"

#include "gtk/gtk.h"

static void CopyAction( GtkButton* self, gpointer data ) {
    logger.log( LogLvl::Warning, "Copy action not implemented yet!");
}

static void DelAction( GtkButton* self, gpointer data ) {
    logger.log( LogLvl::Warning, "Delete action not implemented yet!");
}

TableMenu::TableMenu() {
    auto* builder = gtk_builder_new_from_file( "table_menu.ui" );
    popover = GTK_POPOVER(gtk_builder_get_object( builder, "table_popover" ));

    g_object_ref( popover );
    g_object_unref( builder );

    // logger.log(LogLvl::Error, gtk_widget_get_css_name( GTK_WIDGET(popover)));

    auto* copyBtn = gtk_builder_get_object( builder, "copy" );
    g_signal_connect( copyBtn, "clicked", G_CALLBACK(CopyAction), selected );

    auto* delBtn = gtk_builder_get_object( builder, "delete" );
    g_signal_connect( delBtn, "clicked", G_CALLBACK(DelAction), selected );

}

TableMenu::~TableMenu() {
    g_object_unref( popover );
}
