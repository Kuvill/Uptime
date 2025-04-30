#include <unistd.h>
#include "inc/get_uptime.hpp"
#include "inc/db.hpp"
#include "inc/ram_storage.hpp"
#include "inc/server.hpp"
#include "inc/ipc_interface.hpp"

#include "inc/logger.hpp"

#include <cstdlib>
#include <fstream>
#include <chrono>
#include <exception>
#include <string>
#include <thread>

#include <csignal>
#include <type_traits>

using namespace std::chrono_literals;

// for future: set project dir
ChangeDir ch( std::getenv("HOME") );

// const char* dbName = "res/db/uptime.db";
const char* dbName = ".local/share/uptimer/uptime.db";

Logger logger( ".local/share/uptimer/log.log", LogLvl::Info );

// only for signals
// use them instead of db and storage - scary. mb after 1.0 i will do that
static Database* g_db;
static Storage* g_storage;

void SigHandler( int code ) {
	logger.log(LogLvl::Warning, "Handled signal: ", code, ". Terminate" );

	g_db->dumpStorage( *g_storage );
	exit(0);
}

// MAIN ISSUE: there are path form project root. How to execute from anywhere...?
// i should use switch row and from toolbar view for settings 

// for ipc i'm using socket here. This worser, that pipe (i guess)
// but i think knowing base of sockets will be nice for me

// as moder cpp way i should to pick boost.asio or ZeroMQ

// one more todo: 3) set class of exception:
	// free sqlite memory 

// it take 20 years to 10 gb. so i can just contain. Idle, on day end i want to compose by morning, day eveninng. or custom
// Prevent run several times: use file (create .lock or read an file)
// if locked - brut force all opened program (if i be able to do  this) // to prevent kill -9
int main() {
    // I also want to record browser tab name (or whatever, that contain tab name)
    chdir( std::getenv("HOME") );
	Database db( dbName );

	Storage storage;
	Ips connect;

	// should be ms
    auto sleepDuration = 5s;
	bool useDB = false;

	g_db = &db;
	g_storage = &storage;

	signal( SIGINT, SigHandler );
	signal( SIGABRT, SigHandler );
	signal( SIGTERM, SigHandler );


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

			if( useDB ) {
				db.insertUptimeRecord( FocusInfo() );

			} else {
				storage.insert( FocusInfo() );
			}

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
