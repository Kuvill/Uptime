#include "inc/get_uptime.hpp"

#include <gtk/gtk.h>

#define RECORD_TYPE_ITEM (record_item_get_type() )
G_DECLARE_FINAL_TYPE( RecordItem, record_item, RECORD, ITEM, GObject );

struct _RecordItem {
	GObject parent;
	Name appName;
	size_t uptime;
};

struct _RecordItemClass {
	GObjectClass parent_class;
};

void record_item_get_init( RecordItem* item );
void record_item_class_init( RecordItemClass* );
RecordItem* record_item_new( const Name& name, size_t uptime );

void setup_cb( GtkSignalListItemFactory* factory, GObject* listitem );
void bind_col1_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
void bind_col2_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);

