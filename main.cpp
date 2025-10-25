#include "common/check_unique.hpp"

#include "common/signal_event.hpp"
#include "demon/better_uptime.hpp"
#include "demon/db.hpp"
#include "demon/plugin.hpp"
#include "demon/ram_storage.hpp"
#include "demon/server.hpp"
#include "demon/settings.hpp"
#include "demon/epoll.hpp"

#include "common/logger.hpp"
#include "demon/time_event.hpp"

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <exception>

#include <sys/socket.h>
#include <unistd.h>


using namespace std::chrono_literals;

const char* dbName = "uptime.db";

// has no global constructor
Logger logger;

// use only from signal handler
Storage* g_store;

// use only from signal handler
Database* g_db;

/*
    1. Do clear inside this func:
        +: easy to impl
        -: require global variables e.g. db and store

    2. longjmp
        +: fast, easy to understand, easy to impl
        -: Bad c++ solution: do not call destr, exceptions, and idk what more

    3. signalfd
        +: easy to impl. prevent c style signal from breaking c++ features like exceptions and mb more
        -: a bit slover 

        have to check it fd on every trigger
*/

// later i want to implement 3 way
[[noreturn]] static void SigHandler( int code ) {
    logger.log(LogLvl::Warning, "Handled signal: ", code, ". Terminate" );

    g_db->dumpStorage( *g_store );
    delete_lock_file();

    exit(0);
}

// as moder cpp way i should pick as socket dbus, boost.asio or ZeroMQ

int main() {
    CheckUnique __uniqueChecker__;

#ifndef NOLOG
#ifndef DEBUG
    logger.Init( );
#else
    logger.Init( LogLvl::Info );
#endif
#endif

    [[maybe_unused]] Settings settings;

    Epoll epoll;

	Database db( dbName );
	Storage storage;
	Ips connect;
    [[maybe_unused]] DesktopEnv* env = initDE();
    TimerEvent timer;
    SignalEvent signals;

    g_store = &storage;
    g_db = &db;

    signal(SIGTERM, SigHandler);
    signal(SIGSEGV, SigHandler);
    signal(SIGINT, SigHandler);

	try {
		while( true ) {
            int count = epoll.wait();
            logger.log(LogLvl::Info, "epoll triggered ", count, " sockets"); 

            for( int i = 0; i < count; ++i ) {
                auto event = epoll.ready[i];

                if( event.events & EPOLLIN ) [[likely]] {
                    char buf[1000];
                    auto plugin = static_cast<Plugin*>( event.data.ptr );
                    logger.log(LogLvl::Info, "Triggering ", typeid(*plugin).name());
                    plugin->OnTrigger();
                    read(plugin->getFd(), buf, 1000);
                }

                else if( event.events & EPOLLHUP ) { // socket closed
                    logger.log(LogLvl::Warning, "One of socket closed...");
                    SigHandler( 0 ); // tmp. Should mark module as turned off. 
                    // if i will use json to manage plugins, write cause inside
                    // or btw i can try to reinit module
                }

                else if( event.events & EPOLLERR ) {
                    // should be as function in socket file
                    int error;
                    socklen_t errlen = sizeof( error );
                    getsockopt(event.data.fd, SOL_SOCKET, SO_ERROR, (void*)&error, &errlen );
                    logger.log(LogLvl::Error, strerror(error));
                    // turn off plugin and write cause
                }

            } // event cycle exit
		}
	}

	catch( const std::exception& err ) {
		logger.log(LogLvl::Error, err.what());
	}

	db.dumpStorage( storage );
	return 0;
}

