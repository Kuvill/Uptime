#include <common/logger.hpp>
#include <common/change_dir.hpp>
#include <gui/record_item.hpp>
#include <gui/column_view.hpp>
#include <gui/lazy_load.hpp>
#include <gui/client.hpp>
#include "gui/db.hpp"
#include "gui/context.hpp"

#include <gtk/gtk.h>
#include <libadwaita-1/adwaita.h>
#include <unistd.h>

const char* dbName = "uptime.db";


#ifdef DEBUG
    Logger logger(LogLvl::Info);
#else
    // it still require supervisoring
    Logger logger("logs.log", LogLvl::Info);
#endif

static guint SetupTimer( Context& context ) {
    context.state.createTimer();

    return g_timeout_add_seconds( 1, update_data, &context );
}

// TODO: Add alias into App table
static void activate( GtkApplication* app, gpointer data ) {
    Context* context = static_cast<Context*>( data );

    CheckDirectory();
	GtkBuilder* builder = gtk_builder_new_from_file( "res/gui/main.ui" );
	auto* window = GTK_WINDOW(gtk_builder_get_object( builder, "window" ));
	setup_column_view( builder, *context );
	g_object_unref( builder );

	context->state.mergeStoreUnique( context->db.getRecords(Operators::Eqal, {}) );
    SetupTimer( *context );

	gtk_window_set_application( window , app );
	gtk_window_present( window );
}

int main( int argc, char *argv[] ) {
	AdwApplication* app = adw_application_new("org.kuvil.uptimer", G_APPLICATION_DEFAULT_FLAGS );
    g_object_set(gtk_settings_get_default(),
        "gtk-application-prefer-dark-theme", TRUE,
        NULL);

    Context context{
        DatabaseReader( dbName ),
        Client(),
        State(),
        Settings()
    };

	g_signal_connect( app, "activate", G_CALLBACK( activate ), &context );

	int stat = g_application_run( G_APPLICATION( app ), argc, argv );
	g_object_unref( app );

	return stat;
}
