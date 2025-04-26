#pragma once

#include <gtk/gtk.h>
#include "inc/lazy_load.hpp"

GListStore* setup_column_view( GtkBuilder* builder, State& );
