#include "demon/get_uptime.hpp"
#include "demon/better_uptime.hpp"
#include "common/logger.hpp"
#include "common/aliases.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// !!!!! FIXME
// on send to closed socket will be called SIGPIPE. i should catch it or ignore
// (second will be faster guess)
// MSG_NOSIGNAL

#include <nlohmann/json.hpp>

static const char* DE_ENV_VAR = "XDG_CURRENT_DESKTOP";

recTime_t ps( std::array<char, 6> pid ) {
    logger.log(LogLvl::Warning, "Internal. Used old ps function");

    FILE* mimic;
    const char* ps = "ps -o etimes ";

    char getUptimePlusPID[21]{};
    strcat( getUptimePlusPID, ps );
    strcat( getUptimePlusPID, pid.data() );
    mimic = popen( getUptimePlusPID, "r" );

    std::array<char, 11> uptimeInStr{};

    // read line with garbage 
    fgets(uptimeInStr.data(), uptimeInStr.size(), mimic);
    fgets(uptimeInStr.data(), uptimeInStr.size(), mimic);

    return static_cast<recTime_t>( strtol(uptimeInStr.data(), nullptr, 10) );
}

recTime_t ps( const unsigned char* pid ) {
    auto hacks = reinterpret_cast<const char*>( pid );
    std::array<char, 6> hacks2{};
    std::strncpy( hacks2.data(), hacks, 6 );
    return ps( hacks2 );
}

recTime_t ps( const std::string& pid ) {
    return ps( (unsigned char*)pid.c_str() );
}

/* Version without Inheritance

static ProcessInfo SwayFocusInfo();

static ProcessInfo HyprFocusInfo();

// return empty struct if DE unrecognized
ProcessInfo FocusInfo() {

    const char* de = std::getenv( DE_ENV_VAR );

if( std::strcmp( de, "sway" ) == 0 )
        return SwayFocusInfo();
    
    else if( std::strcmp( de, "Hyprland" ) == 0 )
        return HyprFocusInfo();

    else return {};
}


// for windows or macos we just use other impl if #if
enum class _DE {
    UNKNOWN,
    SWAYWM,
    HYPRLAND,
    PLASMA,
    GNOME
};

constexpr const char* _DEToString( _DE de ) {
    switch( de ) {
        case _DE::UNKNOWN:
            return "Unknown";
            break;

        case _DE::SWAYWM:
            return "Sway";
            break;

        case _DE::HYPRLAND:
            return "Hyprland";
            break;

        case _DE::PLASMA:
            return "KDE Plasma";
            break;

        case _DE::GNOME:
            return "Gnome";
            break;
    }
}

*/



// Press F to SOLID
DesktopEnv* DesktopEnv::checkDE() {
    logger.log(LogLvl::Info, "Recheck current DE");
    char* de( std::getenv( DE_ENV_VAR ) );

    if( !de ) {
        logger.log(LogLvl::Error, "Unable to detect current DE!");
        throw std::runtime_error("Unable to detect current DE!");
    }

    if( strstr(de, ";") != nullptr ) 
        logger.log(LogLvl::Warning, "Deteced multi entry in ", DE_ENV_VAR, " that is unsupporeted yet!");

    if( std::strcmp( de, "sway" ) == 0 ) {
        delete( this );
        return new _SwayDE;
    }
    
    else if( std::strcmp( de, "Hyprland" ) == 0 ) {
        delete( this );
        return new _Hyprland;
    }

    else return this;
}

// I don't recall getFocused to prevent inf recursive
ProcessInfo DesktopEnv::getFocused() {
    logger.log(LogLvl::Warning, "Current Desktop enviroment didn't recognized!");
    return {};
}

void DesktopEnv::castToBase() {}
