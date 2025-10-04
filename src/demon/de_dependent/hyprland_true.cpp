// The all i did before - nothings. i record summary uptime, not time in focus...
// even if you don't use telegram( for example ) and it is in background, fater focus it usage will be big.
// i need reorgonize whole project

#include "demon/better_uptime.hpp"
#include "common/logger.hpp"

#include <cstring>
#include <stdexcept>

#include "demon/epoll.hpp"
#include "demon/plugin.hpp"
#include "sys/socket.h"

static const char* DE_ENV_VAR = "XDG_CURRENT_DESKTOP";

const char HYPR[] = "/hypr/";
const char SOCK[] = "/.socket2.sock";

const char EVENT[] = "activewindowv2";

const char HIS[] = "HYPRLAND_INSTANCE_SIGNATURE";

const char RUN_DIR[] = "XDG_RUNTIME_DIR";

const int INIT_SIZE = 1'000;

_HyprlandTrue::_HyprlandTrue() {
    const char* sign = std::getenv( HIS ) ;
    const char* xdg = std::getenv( RUN_DIR );

    const auto signSize = strlen( sign );
    const auto xdgSize = strlen( xdg );

    sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    std::memcpy( addr.sun_path, xdg, xdgSize - 1 );
    std::memcpy( addr.sun_path + xdgSize - 1, HYPR, sizeof(HYPR) - 1 );
    std::memcpy( addr.sun_path + xdgSize + sizeof(HYPR) - 2, sign, signSize - 1 );
    std::memcpy( addr.sun_path + xdgSize + sizeof(HYPR) + signSize - 3, SOCK, sizeof(SOCK) - 1 );

    if( connect(_fd, reinterpret_cast<sockaddr*>( &addr ), sizeof(addr)) == -1 ) {
        logger.log(LogLvl::Error, "Unable to connect to hyprland socket v2!", "addr: '", addr.sun_path, "'" );
        throw std::runtime_error("Unable to connect to hyprland socket v2!");
    }

    logger.log(LogLvl::Info, "Should be registrated here...");
    Subscribe( _fd, this );
}

_HyprlandTrue::~_HyprlandTrue() {
    logger.log(LogLvl::Info, "Descroying hyprland...");
}

ProcessInfo _HyprlandTrue::getFocused() {
    char buffer[256];
    size_t n = read( _fd, buffer, sizeof(buffer)-1 );
    if( n < 0 ) {
        logger.log(LogLvl::Error, "Unable to read message from hyprland socket v2! ", strerror( errno ));
        throw std::runtime_error("Unable to read message from hyprland socket v2!");
    }

    buffer[n] = 0;
        
    // idle parse strstr to get window id and then use it. But idk how))
    if( auto pos = strstr( buffer, EVENT ); pos != nullptr )
        return _base.getFocused();

    return {};
}

void _HyprlandTrue::InplaceCast( DesktopEnv* self ) {
    self->~DesktopEnv();

    new( self ) _HyprlandTrue;
}

bool _HyprlandTrue::CastCondition() {
    logger.log(LogLvl::Info, "Checking does Hyprland running...");
    char* de( blockingGetEnv( DE_ENV_VAR ) );

    if( !de ) {
        logger.log(LogLvl::Error, "Unable to detect current DE!");
        throw std::runtime_error("Unable to detect current DE!");
    }

    return strstr( "Hyprland", de ) != nullptr;
}

