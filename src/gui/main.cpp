#include <common/logger.hpp>
#include <common/change_dir.hpp>
#include <filesystem>
#include <gui/record_item.hpp>
#include <gui/builder.hpp>
#include <gui/lazy_load.hpp>
#include <gui/client.hpp>
#include "gui/db.hpp"
#include "gui/context.hpp"

#include <gtk/gtk.h>
#include <libadwaita-1/adwaita.h>
#include <stdexcept>
#include <unistd.h>

const char* dbName = "uptime.db";

Context* GContext::ctx = nullptr; 

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
    CheckDirectory();
	GtkBuilder* builder = gtk_builder_new_from_file( "main.ui" );
    
    // hide error handling into builder setup func?
    if( builder == nullptr ) {
        logger.log(LogLvl::Error, "main.ui file was not found! use ninja install or copy it manualy into ", std::filesystem::current_path() );
        throw std::runtime_error("");
    }

	auto* window = GTK_WINDOW(gtk_builder_get_object( builder, "window" ));
	setup_builder( builder );
	g_object_unref( builder );

    GContext::ctx->state.mergeStoreRightVersion( GContext::ctx->db.getRecords(Operators::Eqal, {}) );
    // SetupTimer( *context );

	gtk_window_set_application( window , app );
	gtk_window_present( window );
}

int main( int argc, char *argv[] ) {
	AdwApplication* app = adw_application_new("org.kuvil.uptimer", G_APPLICATION_DEFAULT_FLAGS );
    // g_object_set(gtk_settings_get_default(),
    //    "gtk-application-prefer-dark-theme", TRUE,
    //    NULL);

    // make it global?
    Context context{
        DatabaseReader( dbName ),
        Client(),
        State(),
        Settings()
    };

    GContext::ctx = &context;

	g_signal_connect( app, "activate", G_CALLBACK( activate ), nullptr );

	int stat = g_application_run( G_APPLICATION( app ), argc, argv );
	g_object_unref( app );

	return stat;
}
