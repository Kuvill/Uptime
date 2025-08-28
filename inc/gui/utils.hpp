#pragma once

#include <gtk/gtk.h>

#include "gui/utils/table_menu.hpp"

struct Utils {
    TableMenu table_menu;

    // so ugly
    GtkWindow* window;
};
