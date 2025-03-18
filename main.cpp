#include "inc/get_uptime.hpp"
#include "inc/db.hpp"
#include "inc/ram_storage.hpp"
#include "inc/server.hpp"
#include "ipc_interface.hpp"

#include <iostream>
#include <chrono>
#include <exception>
#include <thread>

const char* dbName = "res/db/uptime.db";

// to do. 1) write catch exception to save all ram data
// 		  2) write signal catch for same

// for ipc i'm using socket here. This worser, that pipe (i guess)
// but i think knowing base of sockets will be nice for me

// as moder cpp way i should to pick boost.asio or ZeroMQ

// one more todo: 3) set class of exception:
	// require ram storage dump
	// don't require
	// !! clear sqlite memory (is require befor death?)

// 		4) write logger

int main() {
	Database db( dbName );
	Storage storage;
	Ips connect;

	int sleepDuration = 5;
	bool useDB = false;


	try {
		while( true ) {

			{
				auto msgType = connect.listen();

				if( msgType == MsgType::start ) {
					db.dumpStorage( storage );
					useDB = true;

				} else if( msgType == MsgType::end )
					useDB = false;
			}

			if( useDB ) {
				db.insertUptimeRecord( FocusInfo() ); // array of bool to chose stat

			} else {
				storage.insert( FocusInfo() );
			}


			std::this_thread::sleep_for( std::chrono::seconds( sleepDuration ) );
		}
	}

	catch( const std::exception& err ) {
		db.dumpStorage( storage );

		// filler
		std::cerr << "Got an error: " << err.what() << '\n';
	}

	return 0;
} 
