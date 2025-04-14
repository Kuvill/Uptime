#include "inc/get_uptime.hpp"
#include "inc/db.hpp"
#include "inc/ram_storage.hpp"
#include "inc/server.hpp"
#include "inc/ipc_interface.hpp"

#include "inc/logger.hpp"

#include <chrono>
#include <exception>
#include <string>
#include <thread>

#include <csignal>

const char* dbName = "res/db/uptime.db";

Logger logger(LogLvl::Info);

// only for signals
// use them instead of db and storage - scary. mb after 1.0 i will do that
Database* g_db;
Storage* g_storage;

void SigHandler( int code ) {
	logger.log(LogLvl::Warning, "Handled signal: ", code, ". Terminate" );

	g_db->dumpStorage( *g_storage );
	exit(0);
}

// MAIN ISSUE: there are path form project root. How to execute from anywhere...?
// i should use switch row and from toolbar view for settings 

// Record description ( title )

// for ipc i'm using socket here. This worser, that pipe (i guess)
// but i think knowing base of sockets will be nice for me

// as moder cpp way i should to pick boost.asio or ZeroMQ

// one more todo: 3) set class of exception:
	// require ram storage dump
	// don't require
	// !! clear sqlite memory (is require befor death?)


// Main questin: Update Writes or make new? 
// first way - cute stat, but potential memory overcup (disk)

// it take 20 years to 10 gb. so i can just contain. Idle, on day end i want to compose by morning, day eveninng. or custom
int main() {
	// Last issue: app with broken name writes as random blob. i have to filter blobs and log Warrning

	Database db( dbName );
	Storage storage;
	Ips connect;

	// should be float / ms
	int sleepDuration = 5;
	bool useDB = false;

	g_db = &db;
	g_storage = &storage;

	// 1) had i take 2 last. 2) what about handle others and just log them?
	signal( SIGINT, SigHandler );
	signal( SIGABRT, SigHandler );
	signal( SIGTERM, SigHandler );


	try {
		while( true ) {
			logger.log( LogLvl::Info, "New Iteration\n" );

			{
				auto msgType = connect.listen();

				// how to incapsulate this?
				if( msgType == MsgType::start_record ) {
					logger.log( LogLvl::Info, "Choosed db");
					db.dumpStorage( storage );
					useDB = true;

				} else if( msgType == MsgType::end_record ) { 
					logger.log( LogLvl::Info, "back to ram");
					useDB = false;

				} else if( msgType == MsgType::change_cd ) {
					logger.log( LogLvl::Info, "changing db cd to: ", connect.getMessage());
					sleepDuration = std::stoi( connect.getMessage() );
				}
			}

			if( useDB ) {
				db.insertUptimeRecord( FocusInfo() );

			} else {
				storage.insert( FocusInfo() );
			}

			std::this_thread::sleep_for( std::chrono::seconds( sleepDuration ) );
		}
	}

	catch( const std::exception& err ) {
		logger.log(LogLvl::Error, err.what());
	}

	logger.log( LogLvl::Info, "memory dump: ", storage );
	db.dumpStorage( storage );
	return 0;
} 
