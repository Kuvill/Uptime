// for future: change popen to sway-ips (7) 

#include "inc/get_uptime.hpp"
#include "inc/logger.hpp"

#include <cstdio>
#include <cstdlib>
#include <array>
#include <cstring>

#define getFocusedInfo "swaymsg -t get_tree |  jq -r '.. | select(.focused? == true) | .app_id, .pid'"

#define getUptimeByPID "ps -o etimes "

// want to create Workspace info

// irrational way. at least one copy. idle to change name back to array
ProcessInfo FocusInfo() {
	ProcessInfo info;
	info.name.resize(40); info.name.clear();
	std::array<char, 40> appName{};

	FILE* mimic;
	mimic = popen( getFocusedInfo, "r" );
	fgets( appName.data(), 40, mimic );

	// if app have no .app_id. idle i should to check other properties (like .window_properties.instance, but idk)
	if( appName[0] == '\0' || !strcmp(appName.data(), "null\n") ) return {{'\0'}, {}};

	// idk why cycle on appName until '\0' or '\n' not workibg
	info.name.append(appName.data()); info.name.pop_back();

	{ // getting uptime by pid
		std::array<char, 6> pid{};
		std::array<char, 11> uptimeInStr{};
		fgets( pid.data(), pid.size(), mimic );

		pclose(mimic);
		char getUptimePlusPID[17]{};
		strcat(getUptimePlusPID, getUptimeByPID);
		strcat(getUptimePlusPID, pid.data());
		mimic = popen( getUptimePlusPID, "r" );
		fgets(uptimeInStr.data(), uptimeInStr.size(), mimic);
		fgets(uptimeInStr.data(), uptimeInStr.size(), mimic);
		info.uptime = strtoll(uptimeInStr.data(), NULL, 10);
		
	}

	pclose(mimic);
	return info;
}


