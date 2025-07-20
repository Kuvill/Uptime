#include "demon/get_uptime.hpp"
#include "common/logger.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

static const char* DE = "XDG_SESSION_DESKTOP";

static const char* getSockAdr();

static long ps_custon( pid_t pid );

static long _ps_custon( pid_t pid ) {
    logger.log(LogLvl::Warning, "Internal. Used old ps function");

    FILE* mimic;
    const char* ps = "ps -o etimes";
    const char* strPid;

    sprintf(strPid, "%d", pid);

    char getUptimePlusPID[20]{};
    strcat(getUptimePlusPID, ps);
    strcat(getUptimePlusPID, strPid);
    mimic = popen( getUptimePlusPID, "r" );



    // read line with garbage 
    fgets(uptimeInStr.data(), uptimeInStr.size(), mimic);
    fgets(uptimeInStr.data(), uptimeInStr.size(), mimic);
    info.uptime = static_cast<recTime_t>( strtol(uptimeInStr.data(), NULL, 10) );
}

static ProcessInfo SwayFocusInfo();

static ProcessInfo HyprFocusInfo();

// return empty struct if DE unrecognized
ProcessInfo FocusInfo() {

    const char* de = std::getenv( DE );

    if( std::strcmp( de, "sway" ) == 0 )
        return SwayFocusInfo();
    
    else if( std::strcmp( de, "Hyprland" ) == 0 )
        return HyprFocusInfo();

    else return {};
}

