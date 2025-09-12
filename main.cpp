#include "common/check_unique.hpp"

#include "demon/better_uptime.hpp"
#include "demon/db.hpp"
#include "demon/ram_storage.hpp"
#include "demon/server.hpp"
#include "demon/settings.hpp"
#include "demon/poll.hpp"

#include "common/logger.hpp"
#include "demon/signals.hpp"
#include "inc/common/change_dir.hpp"

#include <cassert>
#include <exception>
#include <stdexcept>
#include <thread>

#include <csetjmp>

using namespace std::chrono_literals;

const char* dbName = "uptime.db";

#ifdef DEBUG
Logger logger(LogLvl::Info);
#else
Logger logger("logs.log", LogLvl::Info );
#endif

static std::jmp_buf signalHandler;
static LightEvent pollEventFD;
static const uint64_t idkWhatIsIt = 1;

// solution without longjmp: load atomic variable right before and after sleep. in handler set it to true
// Any destructors will be ignored since longjmp used :D
[[noreturn]] static void SigHandler( int code ) { logger.log(LogLvl::Warning, "Handled signal: ", code, ". Terminate" );

    // notify main thread about terminating
    if( write( pollEventFD, &idkWhatIsIt, sizeof(uint64_t) ) == -1 )
        logger.log(LogLvl::Error, "message didn't resived: ", strerror( errno ) );

    longjmp(signalHandler, true );
}

// as moder cpp way i should pick as socket dbus, boost.asio or ZeroMQ

void frequncyPolling( LockNotifier& notifier, Storage& externalStore ) {
    logger.log(LogLvl::Info, "Createing second thread");

    allowInteruptSignals();
    setCallbackForInteruptSignals( SigHandler );

    auto de = initDE();
    Storage store; // on extand i will merge those in module class

    // MHMM Breake whole stack and then goto to random part of code ^^
    // c problems require c solutions Nuahule
    if( setjmp( signalHandler ) )
        goto Finalize; // to save it compile ability: DO NOT PLACE ANYTHINGS WITH DESTRUCTOR IN THIS 
                        // SCOPE LEVEL.

    while( true ) {
        logger.log(LogLvl::Info, "New iteration");
        auto status = notifier._stat.load( std::memory_order::acquire );

        if( status == LockStatus::Terminate )
            break;

        else if( status == LockStatus::SessionLock ) {
            logger.log(LogLvl::Info, "Session locked. going sleep...");

            notifier._stat.wait( LockStatus::SessionLock );
            continue;
        }

        else {
            auto info = de->getFocused();
            if( !info.name.empty() ) {
                store.insert( info );
            }
        }

        std::this_thread::sleep_for( 5s );
    }

Finalize:
    logger.log(LogLvl::Info, "Terminate second thread");
    externalStore = std::move( store );
    assert( store.begin() == store.end() );
}

int main() {
    CheckUnique __uniqueChecker__;
    CheckDirectory(); // just to be sure, that it changed before second thread created

    [[maybe_unused]] Settings settings;

	Database db( dbName );
	Storage storage;
    Storage externalStorage;
	Ips connect;

    LockNotifier notifier;

    blockInteruptSignals();

    std::thread frequncyPollThread( frequncyPolling, std::ref(notifier), std::ref(externalStorage) );
    
    // Require API enhance
    Poll<2> poll( { {pollEventFD, PollEvent::In},
                    {connect, PollEvent::In} });

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

	db.dumpStorage( storage );

    frequncyPollThread.join();
	db.dumpStorage( externalStorage );
	return 0;
}

