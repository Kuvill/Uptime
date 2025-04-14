// unfoturnaly, it can't be an c file
// coz my logger has template functions

// be careful with unrefs

#include <inc/columnView.hpp>
#include <inc/logger.hpp>

/* Record Item */

struct RecordItem {
	GObject parent;
	gchar* appName;
	guint64 uptime;
};

struct RecordItemClass {
	GObjectClass parentClass;
};

#define RECORD_ITEM_TYPE record_item_get_type()
G_DEFINE_TYPE( RecordItem, record_item, G_TYPE_OBJECT );

// i need this to call notify on change
enum {
    PROP_APPNAME = 1,
    PROP_UPTIME,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void record_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec) {
    RecordItem *rec = (RecordItem*)object;

    switch (property_id) {
        case PROP_APPNAME:
            logger.log(LogLvl::Info, "name prop changed!\n");

            g_free(rec->appName);
            rec->appName = g_value_dup_string(value);

            g_object_notify_by_pspec(object, pspec);
            break;
        case PROP_UPTIME:
            logger.log(LogLvl::Info, "uptime prop changed!\n");

            rec->uptime = g_value_get_uint64(value);

            g_object_notify_by_pspec(object, pspec);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void record_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec) {
    RecordItem *rec = (RecordItem*)object;

    switch (property_id) {
        case PROP_APPNAME:
            g_value_set_string(value, rec->appName);
            break;
        case PROP_UPTIME:
            g_value_set_uint64(value, rec->uptime);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}
void record_item_init( RecordItem* item ) {
	item->appName = nullptr;
	item->uptime = 0;
}

void record_item_finalize( GObject* object ) {
	RecordItem* item = (RecordItem*)object;

	if( item->appName )
		g_free( item->appName );

	G_OBJECT_CLASS(record_item_parent_class)->finalize(object);
}

void record_item_class_init( RecordItemClass* klass ) {
    GObjectClass *object_class = G_OBJECT_CLASS( klass );

    object_class->set_property = record_set_property;
    object_class->get_property = record_get_property;

    obj_properties[PROP_APPNAME] = g_param_spec_string("name", "Name", "Name of the application in record", NULL, G_PARAM_READWRITE);
    obj_properties[PROP_UPTIME] = g_param_spec_uint64("uptime", "Uptime", "Uptime of the record", 0, G_MAXUINT64, 0, G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);

	G_OBJECT_CLASS(klass)->finalize = record_item_finalize;
}

RecordItem* record_item_new( const char* appName, guint64 uptime ) {
	RecordItem* item = static_cast<RecordItem*>( g_object_new( RECORD_ITEM_TYPE, nullptr ) );

	item->appName = g_strdup( appName );
	item->uptime = uptime;

	return item;
}


// create widget for an field of RecordItem
// @factory - GtkFactory, that will create an widget
// @item_list - here store our items for MODEL, not factor.
// so just change in list will not present on widget
static void setup_cb( GtkSignalListItemFactory* factory, GtkListItem* item_list ) {
	GtkWidget* label = gtk_label_new( nullptr );
	gtk_label_set_xalign( GTK_LABEL( label ), 0.0f );
	gtk_list_item_set_child( item_list, label );
}

static void on_name_changed(GObject *object, GParamSpec *pspec, GtkWidget *label) {
    const gchar *new_name = static_cast<gchar*>( g_object_get_data(object, "name") );
    g_object_set(label, "label", new_name, NULL);
}

static void on_uptime_changed(GObject *object, GParamSpec *pspec, GtkWidget *label) {
    guint64 new_name;
    g_object_get(object, "uptime", &new_name, NULL);

    gchar strUptime[64]; // stole 1 bit, but hope it will be ok :)
    snprintf( strUptime, 64, "%zu", new_name );

    g_object_set(label, "label", strUptime, NULL);
}

// fill an created by setup widget
// @factory - GtkFactory, that will create an widget
// @item_list - here store our items for MODEL, not factor.
// so just change in list will not present on widget
static void bind_appName_cb( GtkListItemFactory* factory, GtkListItem* listItem ) {
    logger.log(LogLvl::Info, "an record was binded to ColumnView");
    GtkWidget* label = gtk_list_item_get_child( listItem );
    RecordItem* rec = static_cast<RecordItem*>( gtk_list_item_get_item( listItem ) );

    g_signal_connect(rec, "notify::name", G_CALLBACK(on_name_changed), label); 

    gtk_label_set_text( GTK_LABEL( label ), rec->appName );

}

// fill an created by setup widget
// @factory - GtkFactory, that will create an widget
// @item_list - here store our items for MODEL, not factor.
// so just change in list will not present on widget
static void bind_update_cb( GtkListItemFactory* factory, GtkListItem* listItem ) {
    GtkWidget* label = gtk_list_item_get_child( listItem );
    RecordItem* rec = static_cast<RecordItem*>( gtk_list_item_get_item( listItem ) );

    gchar strUptime[64]; // stole 1 bit, but hope it will be ok :)
    snprintf( strUptime, 64, "%zu", rec->uptime );

    g_signal_connect(rec, "notify::uptime", G_CALLBACK(on_uptime_changed), label); 

    gtk_label_set_text( GTK_LABEL( label ), strUptime );
}


/* Record Item */

GListStore* setup_column_view( GtkBuilder* builder ) {
	auto* columnView =  GTK_COLUMN_VIEW(gtk_builder_get_object( builder, "column_view" ));

	auto* appNameCol = GTK_COLUMN_VIEW_COLUMN(gtk_builder_get_object( builder, "app_name_col" ));
	auto* uptimeCol = GTK_COLUMN_VIEW_COLUMN(gtk_builder_get_object( builder, "uptime_col" ));

    GListStore* store = g_list_store_new( RECORD_ITEM_TYPE );

    // test
    RecordItem* item = record_item_new( "firefox", 10 );
    g_list_store_append( store, item);
    g_object_unref( item );

    GtkSingleSelection* model = gtk_single_selection_new( G_LIST_MODEL(store) );
	gtk_column_view_set_model(columnView, GTK_SELECTION_MODEL( model ) );

	// App name factory
	GtkListItemFactory* appNameFactory = gtk_signal_list_item_factory_new();
	g_signal_connect( appNameFactory, "setup", G_CALLBACK(setup_cb), nullptr );
	g_signal_connect( appNameFactory, "bind", G_CALLBACK(bind_appName_cb), nullptr );

	gtk_column_view_column_set_factory( appNameCol, appNameFactory );

	// Uptime factory
	GtkListItemFactory* uptimeFactory = gtk_signal_list_item_factory_new();
	g_signal_connect( uptimeFactory, "setup", G_CALLBACK(setup_cb), nullptr );
	g_signal_connect( uptimeFactory, "bind", G_CALLBACK(bind_update_cb), nullptr );

	gtk_column_view_column_set_factory( uptimeCol, uptimeFactory );

    return store;
}
