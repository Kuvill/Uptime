#include <inc/logger.hpp>
#include <inc/record_item.hpp>
#include <inc/column_view.hpp>
#include <inc/lazy_load.hpp>
#include "inc/db_out.hpp"
#include "inc/context.hpp"

#include <gtk/gtk.h>
#include <libadwaita-1/adwaita.h>
#include <unistd.h>


Logger logger(LogLvl::Info);

static const ChangeDir cd;

const char* dbName = "res/db/uptime.db";

// chars that i want:
// General
//  just rectangles with values as length
//  pie chart
// By app:
//  app uptime change by {date.*}
// By Category:
//  like from AI course: polygon. that more value that acute the angle

// i should contain as third class general settings
// (State shoude contain visual settings, guess)

static guint SetupTimer( Context& context ) {
    context.state.createTimer();

    return g_timeout_add_seconds( 1, update_data, &context );
}

#ifdef DEBUG
static void clicked( GtkButton* self, gpointer data ) {
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
    // Holy Molly... I spend like 4 hours (((
    // to detect, that context dies on activate end

    // btw i can pass it as data
    
    Context* context = new Context{
        DatabaseReader( dbName ),
        State()
    };

	GtkBuilder* builder = gtk_builder_new_from_file( "res/gui/main.ui" );
	auto* window = GTK_WINDOW(gtk_builder_get_object( builder, "window" ));
	setup_column_view( builder, *context );
	g_object_unref( builder );

	context->state.mergeStoreUnique( context->db.getRecords(Operators::Eqal, {}) );
    SetupTimer( *context );

    // ----
#ifdef DEBUG
    GtkWidget* TEST_BTN = gtk_button_new();
    g_signal_connect(TEST_BTN, "clicked", G_CALLBACK(clicked), context->state.getStore());
    auto* window2 = gtk_window_new();
    gtk_window_present( GTK_WINDOW(window2) );
    gtk_window_set_child( GTK_WINDOW(window2), TEST_BTN );
#endif
    // ---

	gtk_window_set_application( window , app );
	gtk_window_present( window );
}

int main( int argc, char *argv[] ) {
	AdwApplication* app = adw_application_new("org.kuvil.uptimer", G_APPLICATION_DEFAULT_FLAGS );

	g_signal_connect( app, "activate", G_CALLBACK( activate ), nullptr );

	int stat = g_application_run( G_APPLICATION( app ), argc, argv );
	g_object_unref( app );

	return stat;
}
