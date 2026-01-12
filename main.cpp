#include "common/change_dir.hpp"
#include "common/check_unique.hpp"

#include "demon/better_uptime.hpp"
#include "demon/db.hpp"
#include "demon/ram_storage.hpp"
#include "demon/server.hpp"
#include "demon/poll.hpp"
#include "demon/settings.hpp"
#include "common/signals_object.hpp"

#include "common/logger.hpp"
#include "demon/time_event.hpp"

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>

#include <sys/socket.h>

using namespace std::chrono_literals;

const char* dbName = "uptime.db";

// has no global constructor
#ifndef NOLOG
Logger logger;
#endif

// use only from signal handler
Storage* g_store;

// use only from signal handler
Database* g_db;

// redflag?
// NotifySettings *settings_;

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

    g_db->insertFrom( *g_store, &Record::info );
    delete_lock_file();

    exit(0);
}

void clearSocket( int fd );

// as moder cpp way i should pick as socket dbus, boost.asio or ZeroMQ

int main( int argc, char** argv ) {
    ChangeDirectoryToHome();

    // Parse args (sry)
    std::string_view overridenConfPath;
    if( argc != 1 )
        for( char* arg = argv[1]; arg != nullptr; ++arg )
            if( std::strcmp( arg++, "-c" ) )
                if( arg != nullptr )
                    overridenConfPath = arg;
    //

    if( !overridenConfPath.empty() ) {
        NotifySettings settings( overridenConfPath );
        settings_ = &settings;

    } else {
        NotifySettings settings;
        settings_ = &settings;
    }

    CheckUnique __uniqueChecker__;

#ifndef NOLOG
#ifndef DEBUG
    // logger.Init( settings.paths.state.string() );
    logger.Init( LogLvl::Info );
#else
    logger.Init( LogLvl::Info );
#endif
#endif


    Signals signals; int signals_id;
	Database db( dbName );
	Storage storage;
	Ips connect;
    TimerEvent timer; int timer_id; // id should be inside class
    
    [[maybe_unused]] DesktopEnv* env = initDE(); int env_id;
    // SignalEvent signals;

    Poll<3> poll {
        {safePFD{signals, PollEvent::In}, &signals_id},
        {safePFD{timer, PollEvent::In}, &timer_id},
        {safePFD{*env, PollEvent::In}, &env_id},
    };

    g_store = &storage;
    g_db = &db;

    signal(SIGTERM, SigHandler);
    signal(SIGSEGV, SigHandler);
    signal(SIGINT, SigHandler);

	try {
		while( true ) {
            int count = poll.wait();
            logger.log(LogLvl::Info, "epoll triggered ", count, " sockets"); 
            // any chanse to code generation? Should do const mapping to variants?
            // for event in events (const - can be unrolled). and visits. 
            // 
            // tuple - truely duck typeing? Yes. Also constexpr lamda expressions :)

            // signals 
            if( poll[signals_id].revents & PollEvent::In ) {
                signals.OnTrigger();
            }
            
            // timer

            if( poll[timer_id].revents & PollEvent::In ) {
                timer.OnTrigger();
                clearSocket( timer );
            }

            if( poll[timer_id].revents & POLLHUP ) [[unlikely]] {
                SigHandler(0);
            }

            if( poll[timer_id].revents & POLLERR ) [[unlikely]] {
                int error;
                socklen_t errlen = sizeof( error );
                getsockopt(timer, SOL_SOCKET, SO_ERROR, (void*)&error, &errlen );
                logger.log(LogLvl::Error, strerror(error));
                SigHandler(0);
            }

            // env
            if( poll[env_id].revents & POLLIN ) {
                timer.OnTrigger();
            }

            if( poll[env_id].revents & POLLHUP ) [[unlikely]] {
                SigHandler(0);
            }

            if( poll[env_id].revents & POLLERR ) [[unlikely]] {
                int error;
                socklen_t errlen = sizeof( error );
                getsockopt(timer, SOL_SOCKET, SO_ERROR, (void*)&error, &errlen );
                logger.log(LogLvl::Error, strerror(error));
                SigHandler(0);
            }
        }
	}

	catch( const std::exception& err ) {
		logger.log(LogLvl::Error, err.what());
	}

	db.insertFrom( storage, &Record::info );
	return 0;
}

