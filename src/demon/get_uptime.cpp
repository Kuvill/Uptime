#include "inc/get_uptime.hpp"

#include <cstdio>
#include <cstdlib>
#include <array>
#include <cstring>

#define getFocusedInfo "swaymsg -t get_tree |  jq -r '.. | select(.focused? == true) | .app_id, .pid'"

#define getUptimeByPID "ps -o etimes "

ProcessInfo FocusInfo() {
	ProcessInfo info;

	FILE* mimic;
	mimic = popen( getFocusedInfo, "r" );
	fgets( info.name.data(), 40, mimic );

	{ // getting uptime by pid
		std::array<char, 5> pid{};
		std::array<char, 11> uptimeInStr{};
		fgets( pid.data(), pid.size(), mimic );

		pclose(mimic);
		char getUptimePlusPID[17]{};
		strcat(getUptimePlusPID, getUptimeByPID);
		strcat(getUptimePlusPID, pid.data());
		mimic = popen( getUptimePlusPID, "r" );
		fgets(uptimeInStr.data(), uptimeInStr.size(), mimic);
		info.uptime = strtoll(uptimeInStr.data(), NULL, 10);

	}

	pclose(mimic);
	return info;
}


