#pragma once

#include "gui/lazy_load.hpp"
#include "gui/context.hpp"

#include <gtk/gtk.h>

GListStore* setup_column_view( GtkBuilder* builder, Context& );
