#pragma once

#include <gtk/gtk.h>

void setup_builder( GtkApplication* app, GtkBuilder* builder );

void setup_stack( GtkBuilder* builder );

void setup_table_navigation( GtkApplication* app, GtkBuilder* builder );

