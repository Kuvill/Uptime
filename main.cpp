#include "common/check_unique.hpp"

#include "demon/better_uptime.hpp"
#include "demon/db.hpp"
#include "demon/ram_storage.hpp"
#include "demon/server.hpp"
#include "demon/settings.hpp"

#include "common/logger.hpp"

#include <cassert>
#include <exception>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <thread>

#include <csignal>
#include <csetjmp>

// Os depended thing
#include <sys/eventfd.h>

using namespace std::chrono_literals;

const char* dbName = "uptime.db";

#ifdef DEBUG
Logger logger(LogLvl::Info);
#else
Logger logger("logs.log", LogLvl::Info );
#endif

// Add begin and commit into sqlite

static std::jmp_buf signalHandler;
static int pollEventFD;
static const uint64_t idkWhatIsIt = 1;

// solution without longjmp: load atomic variable right before and after sleep. in handler set it to true

// Any destructors will be ignored since longjmp used :D
[[noreturn]] static void SigHandler( int code ) { logger.log(LogLvl::Warning, "Handled signal: ", code, ". Terminate" );

    // notify main thread about terminating
    if( write( pollEventFD, &idkWhatIsIt, sizeof(uint64_t) ) == -1 )
        logger.log(LogLvl::Error, "message didn't resived: ", strerror( errno ) );

    longjmp(signalHandler, true );
}

// as moder cpp way i should to pick as socket dbus, boost.asio or ZeroMQ

// one more todo: 3) set class of exception:
	// free sqlite memory

void frequncyPolling( LockNotifier& notifier, Storage& externalStore ) {
    logger.log(LogLvl::Info, "Createing second thread");

    // pollEventFD = eventfd( 0, EFD_CLOEXEC );
    if( pollEventFD == -1 )
        logger.log(LogLvl::Error, strerror( errno ));

    // things to make this thread be able to take signals
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGABRT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_UNBLOCK, &set, NULL);

    // actually tie signals
	signal( SIGINT, SigHandler );
	signal( SIGABRT, SigHandler );
	signal( SIGTERM, SigHandler );

    auto de = initDE();
    // to insta store into db: use notifier, and in main create references
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

    [[maybe_unused]] Settings settings;

	Database db( dbName );
	Storage storage;
    Storage externalStorage;
	Ips connect;

    LockNotifier notifier; notifier._stat = LockStatus::NoLock;

    pollEventFD = eventfd( 0, EFD_CLOEXEC );

    // turn off processing of signals to this thread
    // hide this all postix things
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGABRT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    std::jthread frequncyPollThread( frequncyPolling, std::ref(notifier), std::ref(externalStorage) );
    
    // set up events to listen. (hide this also as posix realisation)
    pollfd fds[2];
    fds[0].fd = pollEventFD;
    fds[0].events = POLLIN;

    fds[1].fd = connect;
    fds[1].events = POLLIN;

	try {
		while( true ) {
            int result = poll(fds, std::size(fds), -1); // -1 - blocking

            if( result < 0 ) [[unlikely]] {
                logger.log( LogLvl::Error, strerror( errno ) );
                throw std::runtime_error("Error in poll routine");
            }

            if( fds[0].revents & POLLIN ) {
                logger.log(LogLvl::Warning, "turn off polling...");
                break;
            }

            for( int i = 1; i < std::size(fds); ++i ) {
                if( fds[i].revents & POLLIN )
                    connect.listen(); 


                // error occured
                if( fds[i].revents & POLLHUP );
                    // goto Finalize;
                    // on socket close
			}
		}
	}

	catch( const std::exception& err ) {
		logger.log(LogLvl::Error, err.what());
	}

	db.dumpStorage( storage );
	db.dumpStorage( externalStorage );
	return 0;
}

/*
              auto info = de->getFocused();

            if( !info.name.empty() ) {
                if( useDB ) {
                    db.insertUptimeRecord( info );

                } else {
                    storage.insert( info );
                }

            } else 
                logger.log(LogLvl::Warning, "Unrecognized application");

            logger.log(LogLvl::Info, "Fall asleep...");
			std::this_thread::sleep_for( sleepDuration );

            auto info = de->getFocused();

            if( !info.name.empty() ) {
                if( useDB ) {
                    db.insertUptimeRecord( info );

                } else {
                    storage.insert( info );
                }
            } else {
            		logger.log(LogLvl::Warning,
                            "The app has no app_id, skipping. (describe: ", info.describe, ")");
            }
            */
