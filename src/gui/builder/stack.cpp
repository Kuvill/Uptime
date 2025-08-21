#include <gtk/gtk.h>

#include <gui/lazy_load.hpp>

void setup_stack( GtkBuilder* builder ) {
    // Fill table from db
    auto* stack = GTK_STACK( gtk_builder_get_object( builder, "body" ) );
    g_signal_connect( stack, "notify::visible-child", G_CALLBACK(on_stack_page_changed), nullptr );
    on_stack_page_changed(stack, 0, nullptr);
}
