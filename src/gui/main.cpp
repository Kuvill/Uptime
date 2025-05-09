#include <inc/logger.hpp>
#include <inc/record_item.hpp>
#include <inc/column_view.hpp>
#include <inc/lazy_load.hpp>
#include "gio/gio.h"
#include "glib-object.h"
#include "inc/db_out.hpp"

#include <gtk/gtk.h>
#include <libadwaita-1/adwaita.h>
#include <unistd.h>

Logger logger(LogLvl::Info);

static const ChangeDir cd;

const char* dbName = "uptime.db";

// chars that i want:
// General
//  just rectangles with values as length
//  pie chart
// By app:
//  app uptime change by {date.*}
// By Category:
//  like from AI course: polygon. that more value that acute the angle

guint SetupTimer( State& state ) {
    GTimer* timer = g_timer_new();
    g_timer_start(timer);
    return g_timeout_add_seconds( 5, update_data, &state );
}

#ifdef DEBUG
void clicked( GtkButton* self, gpointer data ) {
    GListStore* store = (GListStore*)data;

    RecordItem** items = new RecordItem*[2];

    RecordItem* item = record_item_new( "TES", 1 );
    items[0] = item;
    g_list_store_splice( store, 0, 0, (void**)(items), 1 );
    g_object_unref( item );
}
#endif

// Tip: Add alias into App table
static void activate( GtkApplication* app, gpointer data ) {
    DatabaseReader db( "uptime.db" );
    State state;

	GtkBuilder* builder = gtk_builder_new_from_file( "res/gui/main.ui" );
	auto* window = GTK_WINDOW(gtk_builder_get_object( builder, "window" ));
	setup_column_view( builder, state );
	g_object_unref( builder );

	state.mergeStore( db.getRecords(Operators::Eqal, {}) );
    SetupTimer( state );

    // ----
#ifdef DEBUG
    GtkWidget* TEST_BTN = gtk_button_new();
    g_signal_connect(TEST_BTN, "clicked", G_CALLBACK(clicked), state.getStore());
    auto* window2 = gtk_window_new();
    gtk_window_present( GTK_WINDOW(window2) );
    gtk_window_set_child( GTK_WINDOW(window2), TEST_BTN );
#endif
    // ---

	gtk_window_set_application( window , app );
	gtk_window_present( window );
}

int main (int argc, char *argv[]) {
	AdwApplication* app = adw_application_new("org.my.try", G_APPLICATION_DEFAULT_FLAGS );

	g_signal_connect( app, "activate", G_CALLBACK( activate ), nullptr );

	int stat = g_application_run( G_APPLICATION( app ), argc, argv );
	g_object_unref( app );

	return stat;
}
