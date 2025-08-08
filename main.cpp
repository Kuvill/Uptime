#include <unistd.h>
#include "demon/get_uptime.hpp"
#include "demon/better_uptime.hpp"
#include "demon/db.hpp"
#include "demon/ram_storage.hpp"
#include "demon/server.hpp"
#include "common/ipc_interface.hpp"
#include "demon/settings.hpp"

#include "common/logger.hpp"

#include <cstdlib>
#include <chrono>
#include <exception>
#include <string>
#include <thread>

#include <csignal>

using namespace std::chrono_literals;

// 7/10 ERROE FIXME! child process of terminals (pies?) are not closing on error (sock not found) (while it support only sway i should kill it on sway log out)

// set path to project directory (now just ~/.local/share/uptimer)
// UB btw. mb creating singletons in all global classes - better way
static const ChangeDir ch;

// const char* dbName = "res/db/uptime.db";
const char* dbName = "uptime.db";

Logger logger( LogLvl::Info );

// only for signals
// use them instead of db and storage - scary. mb after 1.0 i will do that
static Database* g_db;
static Storage* g_storage;

void SigHandler( int code ) {
	logger.log(LogLvl::Warning, "Handled signal: ", code, ". Terminate" );

	g_db->dumpStorage( *g_storage );
	exit(0);
}

// i should use switch row and from toolbar view for settings

// as moder cpp way i should to pick as socket boost.asio or ZeroMQ

// one more todo: 3) set class of exception:
	// free sqlite memory

// it take 20 years to 10 gb. so i can just contain. Idle, on day end i want to compose by morning, day eveninng. or custom
// Prevent run several times: use file (create .lock or read an file)
// if locked - brut force all opened program (if i be able to do  this) // to prevent kill -9
int main() {
    // I also want to record browser tab name (or whatever, that contain tab name)
	Database db( dbName );

    Settings settings;
	Storage storage;
	Ips connect;
    DesktopEnv* de = new DesktopEnv;
    de = de->checkDE();
	// should be ms
    auto sleepDuration = 5s;
	bool useDB = false;

	g_db = &db;
	g_storage = &storage;

	signal( SIGINT, SigHandler );
	signal( SIGABRT, SigHandler );
	signal( SIGTERM, SigHandler );

    // save connection with sway socket forever

	try {
		while( true ) {
			logger.log( LogLvl::Info, "New Iteration\n" );

			{
				auto msgType = connect.listen();

				if( msgType == MsgType::start_record ) {
					logger.log( LogLvl::Info, "Choosed db");
					db.dumpStorage( storage );
					useDB = true;

				} else if( msgType == MsgType::end_record ) {
					logger.log( LogLvl::Info, "back to ram");
					useDB = false;

				} else if( msgType == MsgType::change_cd ) {
					logger.log( LogLvl::Info, "changing db cd to: ", connect.getMessage());
					sleepDuration = static_cast<std::chrono::seconds>(
                            std::stoi( connect.getMessage() ) );
				}
			}

            // FIXME i wan't call checkDE every time, when it return empty state
            // possible solutions:
            // * reserve for de variable max possible space (union?) -> call change
            //      realisation right when detected with placement new
            // * return std::except with enum code error: empty, wrong de
            // * use variant as polimorphism
            auto info = de->getFocused();

            if( info != ProcessInfo{} ) {
                if( useDB ) {
                    db.insertUptimeRecord( info );

                } else {
                    storage.insert( info );
                }

            } else 
                de = de->checkDE();

            logger.log(LogLvl::Info, "Fall asleep...");
			std::this_thread::sleep_for( sleepDuration );
		}
	}

	catch( const std::exception& err ) {
		logger.log(LogLvl::Error, err.what());
	}

	logger.log( LogLvl::Info, "memory dump: ", storage );
	db.dumpStorage( storage );
	return 0;
}
