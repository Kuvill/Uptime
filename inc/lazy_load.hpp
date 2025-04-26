#pragma once

#include <gtk/gtk.h>

void on_stack_page_changed( GtkStack* stack, GParamSpec* pspec, gpointer data );

gboolean update_data( gpointer data );
