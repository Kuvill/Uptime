#include <inc/logger.hpp>
#include <inc/record_item.hpp>
#include <inc/column_view.hpp>
#include <inc/lazy_load.hpp>

#include <gtk/gtk.h>
#include <libadwaita-1/adwaita.h>

Logger logger(LogLvl::Info);

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

// Add alias into App
static void activate( GtkApplication* app, gpointer data ) {
    State state;

	GtkBuilder* builder = gtk_builder_new_from_file( "res/gui/main.ui" );
	auto* window = GTK_WINDOW(gtk_builder_get_object( builder, "window" ));
	setup_column_view( builder, state );
	g_object_unref( builder );

    SetupTimer( state );
    

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

