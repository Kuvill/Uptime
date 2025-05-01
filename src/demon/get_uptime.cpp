// for future: change popen to sway-ips (7) 

#include "inc/get_uptime.hpp"
#include "inc/logger.hpp"

#include <cstdio>
#include <cstdlib>
#include <array>
#include <cstring>

static const char* getFocusedInfo = "swaymsg -t get_tree |  jq -r '.. | select(.focused? == true) | .app_id, .name, .pid'";

static const char* getUptimeByPID = "ps -o etimes ";

// want to create Workspace info

// irrational way. at least one copy. idle to change name back to array
ProcessInfo FocusInfo() {
	ProcessInfo info;
	info.name.resize(40); info.name.clear();
	std::array<char, 40> appName{};
    std::array<char, 100> describe{}; 

    // just hope, that swaymsg member size - 256
    std::array<char, 256> garbage;

	FILE* mimic;
	mimic = popen( getFocusedInfo, "r" );

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
    info.name.append(appName.data()); info.name.pop_back();

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

	pclose(mimic);
	return info;
}

