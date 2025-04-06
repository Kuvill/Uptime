#include <gtk/gtk.h>
#include <libadwaita-1/adwaita.h>

static void activate( GtkApplication* app, gpointer data ) {
	GtkBuilder* builder = gtk_builder_new_from_file( "res/gui/main.ui" );

	GObject* window = gtk_builder_get_object( builder, "window" );
	gtk_window_set_application( GTK_WINDOW( window ), app );
	
    GSList* objects = gtk_builder_get_objects(builder);

	g_object_unref( builder );


	gtk_window_present( GTK_WINDOW( window ) );
	// g_slist_free( objects ); // it done by window?
}

int main (int argc, char *argv[]) {
	AdwApplication* app = adw_application_new("org.my.try", G_APPLICATION_DEFAULT_FLAGS );

	g_signal_connect( app, "activate", G_CALLBACK( activate ), nullptr );

	int stat = g_application_run( G_APPLICATION( app ), argc, argv );
	g_object_unref( app );

	return stat;
}

