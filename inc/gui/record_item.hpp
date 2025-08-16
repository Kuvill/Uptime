#pragma once

#include "glib.h"
#include "common/time.hpp"

#include <gtk/gtk.h>

#define RECORD_ITEM_TYPE (record_item_get_type())
G_DECLARE_FINAL_TYPE(RecordItem, record_item, RECORD, ITEM, GObject)


struct _RecordItem {
    GObject parent;
    gchar* appName;
    recTime_t uptime;
    recTime_t recTimer;
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
RecordItem* record_item_new();

int RecordItemNameCompare( const void* lhs, const void* rhs, void* );
int RecordItemUptimeCompare( const void* lhs, const void* rhs, void* );

bool RecordItemNameLess( RecordItem* lhs, RecordItem* rhs );
int RecordItemOnlyStrCmp(  const void* lhs, const void* rhs );
bool RecordItemEqual( RecordItem* lhs, RecordItem* rhs );
gboolean RecordItemNameEqual( gconstpointer lhs, gconstpointer rhs );
