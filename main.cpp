#include "common/check_unique.hpp"

#include "demon/better_uptime.hpp"
#include "demon/db.hpp"
#include "demon/ram_storage.hpp"
#include "demon/server.hpp"
#include "demon/settings.hpp"
#include "demon/poll.hpp"

#include "common/logger.hpp"
#include "demon/time_event.hpp"
#include "inc/common/change_dir.hpp"

#include <cassert>
#include <exception>
#include <stdexcept>


using namespace std::chrono_literals;

const char* dbName = "uptime.db";

#ifdef DEBUG
Logger logger(LogLvl::Info);
#else
Logger logger("logs.log", LogLvl::Info );
#endif

[[noreturn]] static void SigHandler( int code ) {
    logger.log(LogLvl::Warning, "Handled signal: ", code, ". Terminate" );

    abort();
}

// as moder cpp way i should pick as socket dbus, boost.asio or ZeroMQ

int main() {
    CheckUnique __uniqueChecker__;
    CheckDirectory(); // just to be sure, that it changed before second thread created

    [[maybe_unused]] Settings settings;

	Database db( dbName );
	Storage storage;
	Ips connect;
    DesktopEnv* env = initDE();
    TimerEvent timer;

    // Require API enhance
    Poll<2> poll({ {connect, PollEvent::In},
                    {timer, PollEvent::In} });

    // if timer triggered: trigger all
    // if triggered socket: 

	try {
		while( true ) {
            int result = poll.listen();

            if( result < 0 ) [[unlikely]] {
                logger.log( LogLvl::Error, strerror( errno ) );
                throw std::runtime_error("Error in poll routine");
            }

            if( poll[0].revents & POLLIN ) {
                logger.log(LogLvl::Warning, "turn off polling...");
                break;
            }

            for(  size_t i = 1; i < poll.size(); ++i ) {
                if( poll[i].revents & POLLIN )
                    connect.listen(); 

                // on error occured e.g. socket close
                if( poll[i].revents & POLLHUP ) {}
			}
		}
	}

	catch( const std::exception& err ) {
		logger.log(LogLvl::Error, err.what());
	}

Finalize:
	db.dumpStorage( storage );
	return 0;
}

