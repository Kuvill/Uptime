#include <inc/logger.hpp>
#include <inc/columnView.hpp>

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

// lazy builder init, that just create everythings from builder
static void activate( GtkApplication* app, gpointer data ) {
	GtkBuilder* builder = gtk_builder_new_from_file( "res/gui/main.ui" );

	auto* window = GTK_WINDOW(gtk_builder_get_object( builder, "window" ));

	setup_column_view( builder );

	g_object_unref( builder );


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

