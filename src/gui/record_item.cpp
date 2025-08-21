// unfoturnaly, it can't be an c file
// coz my logger has template functions

// be careful with unrefs

#include <gui/record_item.hpp>
#include <common/logger.hpp>
#include "glib.h"

#include <gtk/gtk.h>

/* Record Item */

G_DEFINE_TYPE( RecordItem, record_item, G_TYPE_OBJECT );
G_DEFINE_TYPE( RawRecordItem, raw_record_item, G_TYPE_OBJECT );

bool RecordItemEqual( RecordItem* lhs, RecordItem* rhs ) {
    return RecordItemOnlyStrCmp(lhs, rhs) == 0 &&
        lhs->uptime == rhs->uptime;
}

// bool
gboolean RecordItemNameEqual( gconstpointer lhs, gconstpointer rhs ) {
    return g_strcmp0( reinterpret_cast<const RecordItem*>( lhs )->appName, 
           reinterpret_cast< const RecordItem*>( rhs )->appName) == 0;
}

bool RecordItemNameLess( RecordItem* lhs, RecordItem* rhs ) {
    if( RecordItemNameCompare( lhs, rhs, nullptr ) < 0 )
        return true;
    return false;
}

gint RecordItemOnlyStrCmp( gconstpointer lhs, gconstpointer rhs ) {
    return g_strcmp0( reinterpret_cast<const RecordItem*>( lhs )->appName, 
           reinterpret_cast< const RecordItem*>( rhs )->appName);
}

// second compare param - uptimer
gint RecordItemNameCompare( gconstpointer lhs, gconstpointer rhs, gpointer data ) {
    int result = g_strcmp0( reinterpret_cast<const RecordItem*>( lhs )->appName,
            reinterpret_cast< const RecordItem*>( rhs )->appName );

    // if name is same, compare by uptime
    if( result == 0 ) {
        if( reinterpret_cast<const RecordItem*>( lhs )->uptime >
            reinterpret_cast<const RecordItem*>( rhs )->uptime ) {
            return 1;
        } else
            return -1;
    }

    return result;
}

gint RecordItemUptimeCompare( gconstpointer lhs, gconstpointer rhs, gpointer data ) {
    return reinterpret_cast<const RecordItem*>( lhs )->uptime.count() -
           reinterpret_cast< const RecordItem*>( rhs )->uptime.count();
}

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

            rec->uptime = toRecTime(g_value_get_uint64(value));

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
            g_value_set_uint64(value, rec->uptime.count());
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}
void record_item_init( RecordItem* item ) {
	item->appName = nullptr;
	item->uptime = toRecTime(0);
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
	item->uptime = toRecTime(uptime);

	return item;
}

// c++ function overload, but since i use logger it is ok
RecordItem* record_item_new() {
	return static_cast<RecordItem*>( g_object_new( RECORD_ITEM_TYPE, nullptr ) );
}

// --------- RAW

void raw_record_item_init( RawRecordItem* item ) {
	item->appName = nullptr;
	item->recTimer = toRecTime(0);
}

void raw_record_item_finalize( GObject* object ) {
	RawRecordItem* item = (RawRecordItem*)object;

	if( item->appName )
		g_free( item->appName );

	G_OBJECT_CLASS(record_item_parent_class)->finalize(object);
}

void raw_record_item_class_init( RawRecordItemClass* klass ) {
	G_OBJECT_CLASS(klass)->finalize = raw_record_item_finalize;
}

RawRecordItem* raw_record_item_new( const char* appName, guint64 uptime ) {
	RawRecordItem* item = static_cast<RawRecordItem*>( g_object_new( RECORD_ITEM_TYPE, nullptr ) );

	item->appName = g_strdup( appName );
	item->recTimer = toRecTime(uptime);

	return item;
}

// c++ function overload, but since i use logger it is ok
RawRecordItem* raw_record_item_new() {
	return static_cast<RawRecordItem*>( g_object_new( RECORD_ITEM_TYPE, nullptr ) );
}
