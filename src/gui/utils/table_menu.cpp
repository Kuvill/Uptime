#include "gui/utils/table_menu.hpp"
#include "common/change_dir.hpp"
#include "glib.h"
#include "gtk/gtk.h"

static void CopyAction( GtkButton* self, gpointer data ) {
    logger.log( LogLvl::Warning, "Copy action not implemented yet!");
}

static void DelAction( GtkButton* self, gpointer data ) {
    logger.log( LogLvl::Warning, "Delete action not implemented yet!");
}

TableMenu::TableMenu() {
    CheckDirectory();

    auto* builder = gtk_builder_new_from_file( "table_menu.ui" );
    popover = GTK_POPOVER(gtk_builder_get_object( builder, "table_popover" ));

    auto* copyBtn = gtk_builder_get_object( builder, "copy" );
    g_signal_connect( copyBtn, "clicked", G_CALLBACK(CopyAction), selected );

    auto* delBtn = gtk_builder_get_object( builder, "delete" );
    g_signal_connect( delBtn, "clicked", G_CALLBACK(DelAction), selected );


    g_object_unref( builder );

}

TableMenu::~TableMenu() {
    logger.log(LogLvl::Error, "deleting table menu");
    g_object_unref( popover );
}
