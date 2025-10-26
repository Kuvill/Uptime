#include <common/logger.hpp>
#include <common/change_dir.hpp>
#include <gui/record_item.hpp>
#include <gui/builder.hpp>
#include <gui/lazy_load.hpp>
#include <gui/client.hpp>
#include "gui/db.hpp"
#include "gui/context.hpp"
#include "gui/state.hpp"

#include <gtk/gtk.h>
#include <libadwaita-1/adwaita.h>
#include <unistd.h>

const char* dbName = "uptime.db";

#ifndef NOLOG
    Logger logger;
#endif

static guint SetupTimer( Context& context ) {
    context.state.createTimer();

    return g_timeout_add_seconds( 1, update_data, &context );
}

// TODO: Add alias into App table
static void activate( GtkApplication* app, gpointer data ) {

	GtkBuilder* builder = gtk_builder_new_from_file( "main.ui" );
	setup_builder( app, builder );

	auto* window = GTK_WINDOW(gtk_builder_get_object( builder, "window" ));
	gtk_window_set_application( window , app );
    Context::get()->utils.window = window;

	g_object_unref( builder );

    // Context::get()->state.changeModel( Duration::ByDay );
    // logger.log(LogLvl::Warning, "Dates: ", Context::get()->state._bound.from, "-", Context::get()->state._bound.to);

    Context::get()->state.mergeStoreRightVersion( Context::get()->db.getRecords() );
    // SetupTimer( *context );

	gtk_window_present( window );
}

int main( int argc, char *argv[] ) {
    ChangeDirectoryToHome();

#ifndef NOLOG
#ifdef DEBUG
    logger.Init( LogLvl::Info );
#else
    logger.Init( ".local/state/uptimer/", LogLvl::Error );
#endif
#endif

	AdwApplication* app = adw_application_new("org.kuvil.uptimer", G_APPLICATION_DEFAULT_FLAGS );
	g_signal_connect( app, "activate", G_CALLBACK( activate ), nullptr );
    logger.log(LogLvl::Info, "Init...");

	int stat = g_application_run( G_APPLICATION( app ), argc, argv );
	g_object_unref( app );

    delete Context::self;

	return stat;
}
