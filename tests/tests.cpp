#include "tests/tests.hpp"
#include "inc/db.hpp"


void run() {
	if( checkDB && checkRam && checkServer() )
		return;

	logger.log( LogLvl::Error, "Unit test failed. Terminate.");
	exit(1);
}

bool checkDB() {
}

bool checkRam() {
	
}

bool checkServer() {

}

