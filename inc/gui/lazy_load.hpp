#pragma once

#include <gtk/gtk.h>

#include "common/time.hpp"
#include "gui/record_item.hpp"
#include "gui/db.hpp"

// that is strut hell imo. i have to orginize them.
// never worked with gui befor, i have to change


void onDurationAction( GSimpleAction* self, GVariant* param, gpointer data );

void on_stack_page_changed( GtkStack* stack, GParamSpec* pspec, gpointer data );

gboolean update_data( gpointer data );
