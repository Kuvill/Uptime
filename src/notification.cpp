#include <common/notificaion.hpp>
#include <common/logger.hpp>

#include <libnotify/notify.h>

// create array since it is't cpu bottleneck
static bool inited = false;
static bool works = true;

// meh. guess, required mutex
void notificate( const char* what ) {
    if( !works ) return;

    if( !inited ) {
        inited = true;

        // or just store one more static bool "works"
        if( !notify_init("org.kuvil.Uptimer") ) {
            works = false;
            logger.log(LogLvl::Warning, "Unable to init notifications");
            return;
        }

    }

    auto message = notify_notification_new( "Uptimer error!", what, nullptr );

    notify_notification_set_timeout(message, -1);

    if( !notify_notification_show(message, nullptr) ) {
        works = false;
        logger.log(LogLvl::Warning, "Unable to send notification");
    }

    g_object_unref( message );
}

