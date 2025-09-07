#include "common/check_unique.hpp"
#include "demon/better_uptime.hpp"
#include "demon/db.hpp"
#include "demon/ram_storage.hpp"
#include "demon/server.hpp"
#include "demon/settings.hpp"

#include "common/logger.hpp"

#include <chrono>
#include <exception>
#include <sys/poll.h>
#include <thread>

#include <csignal>

using namespace std::chrono_literals;

const char* dbName = "uptime.db";

#ifdef DEBUG
Logger logger(LogLvl::Info);
#else
Logger logger("logs.log", LogLvl::Info );
#endif

// only for signals
static Database* g_db;
static Storage* g_storage;

static void SigHandler( int code ) {
	logger.log(LogLvl::Warning, "Handled signal: ", code, ". Terminate" );

	g_db->dumpStorage( *g_storage );

    delete_lock_file();
	exit(0);
}

// as moder cpp way i should to pick as socket dbus, boost.asio or ZeroMQ

// one more todo: 3) set class of exception:
	// free sqlite memory

void frequncyPolling( const LockNotifier& notifier ) {
    logger.log(LogLvl::Info, "Createing second thread");

    while( true ) {
        auto status = notifier._stat.load( std::memory_order::acquire );

        if( status == LockStatus::Terminate )
            break;

        else if( status == LockStatus::SessionLock ) {
            logger.log(LogLvl::Info, "Session locked. going sleep...");

            notifier._stat.wait( LockStatus::SessionLock );
            continue;
        }

        else {
        }

    }

    logger.log(LogLvl::Info, "Terminate second thread");
    // here move ram_storage into heap inter thread variables (mb this will be an tip for opitmizer)
}

int main() {
    CheckUnique __uniqueChecker__;

    // cringe? Cringe
    // I use it make sure, that DE is init
    // Better solution - sleep after failed getenv
    logger.log(LogLvl::Info, "Fall asleep for a while to give DE load...");
    std::this_thread::sleep_for(4s);
	Database db( dbName );

    [[maybe_unused]] Settings settings;
	Storage storage;
	Ips connect;
    // To make it unique_ptr i need some hacks
    // But it needn't
    registrateAll();
    void* reserve = malloc(sizeForDE());
    DesktopEnv* de = new(reserve) DesktopEnv; 
    LockNotifier notifier;
    de->checkDE();

    auto sleepDuration = 5s;
	bool useDB = false;

    std::jthread frequncyPollThread( frequncyPolling, notifier );
    
    pollfd fds[2];
    fds[0].fd = connect;
    fds[0].events = POLLOUT | POLLWRBAND;
    fds[0].fd = -1;

	g_db = &db;
	g_storage = &storage;

	signal( SIGINT, SigHandler );
	signal( SIGABRT, SigHandler );
	signal( SIGTERM, SigHandler );

	try {
		while( true ) {
            int result = poll(fds, std::size(fds), -1); // -1 - blocking

            for( int i = 0; i < 2; ++i ) {
                if( fds[i].revents & POLLOUT ) {
                    connect.listen(); 

                } else if ( fds[i].revents & POLLWRBAND ) // high priority 
                    logger.log(LogLvl::Error, "high priority socket msg didn't readed");

                // error occured
                if( fds[i].revents & POLLHUP ) {

                }
			}

            // FIXME i wan't call checkDE every time, when it return empty state
            // possible solutions:
            // * reserve for de variable max possible space (union?) -> call change
            //      realisation right when detected with placement new
            // * return std::except with enum code error: empty, wrong de
            // * use variant as polimorphism
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

            logger.log(LogLvl::Info, "Fall asleep...");
			std::this_thread::sleep_for( sleepDuration );
		}
	}

	catch( const std::exception& err ) {
		logger.log(LogLvl::Error, err.what());
	}

    notifier._stat.store( LockStatus::Terminate );
    notifier._stat.notify_one();
    frequncyPollThread.join();

	logger.log( LogLvl::Info, "memory dump: ", storage );
	db.dumpStorage( storage );
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

            */
