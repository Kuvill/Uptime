#pragma once

#include "inc/lazy_load.hpp"
#include "inc/context.hpp"

#include <gtk/gtk.h>

GListStore* setup_column_view( GtkBuilder* builder, Context& );
