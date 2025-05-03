#pragma once

#include <gtk/gtk.h>

#include "inc/time.hpp"
#include "inc/record_item.hpp"
#include "inc/db_out.hpp"

// that is strut hell imo. i have to orginize them.
// never worked with gui befor, i have to change


void on_stack_page_changed( GtkStack* stack, GParamSpec* pspec, gpointer data );

gboolean update_data( gpointer data );
