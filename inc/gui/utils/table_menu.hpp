#pragma once

#include <gtk/gtk.h>

struct TableMenu {
    GtkPopover* popover;
    // btw i should store not only Item, but possition to have access to 
    // helper members as rec_time to find the record in db
    GtkListItem* selected;

    TableMenu();

    ~TableMenu();
};
