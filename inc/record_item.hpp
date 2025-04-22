#pragma once

#include <gtk/gtk.h>

#define RECORD_ITEM_TYPE (record_item_get_type())
G_DECLARE_FINAL_TYPE(RecordItem, record_item, RECORD, ITEM, GObject)

struct _RecordItem {
    GObject parent;
    gchar* appName;
    guint64 uptime;
};

struct _RecordItemClass {
    GObjectClass parent_class;
};

// i need this to call notify on change
enum {
    PROP_APPNAME = 1,
    PROP_UPTIME,
    N_PROPERTIES
};

RecordItem* record_item_new( const char* appName, guint64 uptime );

