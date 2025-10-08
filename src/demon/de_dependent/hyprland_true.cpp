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
    logger.log(LogLvl::Info, "Hyprland detected!");
    if(( _fd = socket( AF_UNIX, SOCK_STREAM, 0 ) ); _fd < 0 ) {
        logger.log(LogLvl::Error, "Unable to create socket!!");
        throw std::runtime_error("Unable to create socket!!");
        // get sockerr
    }

    const char* sign = std::getenv( HIS ) ;
    const char* xdg = std::getenv( RUN_DIR );

    const auto signSize = strlen( sign );
    const auto xdgSize = strlen( xdg );
    constexpr const auto HYPRSize = sizeof(HYPR) - 1;
    constexpr const auto SOCKSize = sizeof(SOCK); // with zero terminate at the end

    sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    std::memcpy( addr.sun_path, xdg, xdgSize );
    std::memcpy( addr.sun_path + xdgSize, HYPR, HYPRSize );
    std::memcpy( addr.sun_path + xdgSize + HYPRSize, sign, signSize );
    std::memcpy( addr.sun_path + xdgSize + HYPRSize + signSize, SOCK, SOCKSize );

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

