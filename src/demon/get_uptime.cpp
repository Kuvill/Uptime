// for future: change popen to sway-ips (7) 

#include "demon/get_uptime.hpp"
#include "common/logger.hpp"

#include <bit>
#include <cstdio>
#include <cstdlib>
#include <array>
#include <cstring>
#include <cassert>
#include <ostream>

#include <stdexcept>
#include <sys/times.h>
#include <unistd.h>

static const char* getFocusedInfo = "swaymsg -t get_tree |  jq -r '.. | select(.focused? == true) | .app_id, .name, .pid'";

static const char* getUptimeByPID = "ps -o etimes ";

std::ostream& operator<<( std::ostream& os, const ProcessInfo& info ) {
    os << "{'" << (info.name.data() ? info.name.data() : "") <<
        "' (" << info.describe << "), " << info.uptime;

    return os;
}


struct AUTOFILE {
    FILE* file;

    AUTOFILE( FILE* file ) : file(file) {};

    ~AUTOFILE() {
        pclose( file );
    }

    operator FILE*() const {
        return file;
    }
};

// <magic-string> <payload-length> <payload-type> <payload>
// magic-string - i3-ipc
const int MSG_SIZE = 14;
// use when c++26 became reallity...
static consteval const std::array<char, MSG_SIZE> GetWorkspacesQuerryNextgen() {
    const uint32_t len = 0, type = 1;

    std::string querry = "i3-ipc";
    // should check endian btw
    querry.append( reinterpret_cast<const char*>(&len), sizeof(len) );
    querry.append( reinterpret_cast<const char*>(&type), sizeof(type) );

    std::array<char, MSG_SIZE> result;
    for( int i = 0; i < MSG_SIZE; ++i ) 
        result[i] = querry[i];

    return result;
}

static const std::byte WorkspacesQuerry[] = {
    std::byte(105), std::byte(51), std::byte(45), // i3-
    std::byte(105), std::byte(112), std::byte(99), // ipc
    std::byte(0), std::byte(0), std::byte(0), std::byte(0), // 0
    std::byte(4), std::byte(0), std::byte(0), std::byte(0), // 4
};

// should be free
static const char* getSockAdr() {
    auto result = std::getenv( "SWAYSOCK" );

    if( result )
        return result;

    FILE* mimic = popen( "sway --get-socketpath", "r" );
    assert( mimic != nullptr );

    std::array<char, 256> secondTry;
    char* bufPtr = fgets( secondTry.data(), secondTry.size(), mimic );
    if( bufPtr == nullptr ) 
        throw std::runtime_error( "Socket addr is unknown. Have to use old method" );

    result = new char( strlen( secondTry.data() ) );
    return result;
}

// wrong impl??? (troubles with finding uptime in proc)

// when fix issue upon, chnage istringstream to find ' '. coz:
// 1. second argument in /proc can be multi-world.
// 2. preformace reason
static long ps_custom( pid_t pid ) {
    std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
    if (!stat_file.is_open()) {
        throw std::runtime_error("Internal error: process by pid not found");
    }

    std::string line;
    std::getline(stat_file, line);
    std::istringstream iss(line);
    std::string token;
    int field_count = 0;
    long start_time = 0;

    // The 22nd field in /proc/[pid]/stat is the start time
    while (iss >> token) {
        if (field_count == 21) { // 0-based index, 22nd field
            start_time = std::stol(token);
            break;
        }
        field_count++;
    }

    stat_file.close();

    // Get the clock ticks per second
    long clock_ticks = sysconf(_SC_CLK_TCK);
    long elapsed_time = (times(nullptr) - start_time) / clock_ticks;

    return elapsed_time;

}


// i should talk with sway with socket over swaymsg
// over jq use json lib

// want to create Workspace info

// irrational way. at least one copy. idle to change name back to array
// return {{'\0'}, {}} if appname cannot be readen
ProcessInfo FocusInfo() {
	ProcessInfo info;
	// info.name.resize(40); info.name.clear();
	Name& appName = info.name;
    std::array<char, 100> describe{}; 

    // just hope, that swaymsg member size - 256
    std::array<char, 256> garbage;

	AUTOFILE mimic = popen( getFocusedInfo, "r" );

    assert( mimic != nullptr );

    // read appName
	char* bufPtr = fgets( appName.data(), appName.size(), mimic );
    if( bufPtr == nullptr ) return {{'\0'}, {}};

    int strSize = strlen( appName.data() );
    if( strSize != 0 && appName[strSize-1] != '\n' )
        fgets( garbage.data(), garbage.size(), mimic );

    // read description
    bufPtr = fgets( describe.data(), describe.size(), mimic );
    if( bufPtr == nullptr ) return {{'\0'}, {}};

    strSize = strlen( describe.data() );
    if( strSize != 0 && describe[strSize-1] != '\n' )
        fgets( garbage.data(), garbage.size(), mimic );

	// if app have no .app_id
    // idle give user choose: insert line with emprt name and describe
    if( appName[0] == '\0' || !strcmp(appName.data(), "null\n") )
        return {{'\0'}, {}};

	// idk why cycle on appName until '\0' or '\n' not workibg
    // info.name.append(appName.data()); info.name.pop_back();

    if( describe.data() != 0 ) {
        info.describe.append( describe.data() );
        info.describe.pop_back();
    }

	{ // getting uptime by pid
		std::array<char, 6> pid{};
		std::array<char, 11> uptimeInStr{};

		fgets( pid.data(), pid.size(), mimic );
		pclose(mimic);

		char getUptimePlusPID[17]{};
		strcat(getUptimePlusPID, getUptimeByPID);
		strcat(getUptimePlusPID, pid.data());
		mimic = popen( getUptimePlusPID, "r" );

        // read line with garbage 
		fgets(uptimeInStr.data(), uptimeInStr.size(), mimic);
		fgets(uptimeInStr.data(), uptimeInStr.size(), mimic);
		info.uptime = static_cast<recTime_t>( strtol(uptimeInStr.data(), NULL, 10) );
	}

	return info;
}

