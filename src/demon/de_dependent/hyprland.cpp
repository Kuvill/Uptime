#include "demon/better_uptime.hpp"
#include "common/logger.hpp"
#include "common/aliases.hpp"

#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstdlib>

// FIXME!!!
// that freak close connect after a few ??? of inactive. 
// idle i want to find disscusion about it in github

static const char* DE_ENV_VAR = "XDG_CURRENT_DESKTOP";

const char HYPR[] = "/hypr/";
const char SOCK[] = "/.socket.sock";

const char MSG[] = "activewindow";

// Should be allocated once in constr, in sway also btw
const int INIT_SIZE = 1'000;

_Hyprland::_Hyprland() {
    logger.log(LogLvl::Info, "Hyprland detected!");

    addr.sun_family = AF_UNIX;

    char* sign( blockingGetEnv("HYPRLAND_INSTANCE_SIGNATURE") );
    // should be quered once
    char* xdg( blockingGetEnv("XDG_RUNTIME_DIR"));

    const auto signSize = strlen( sign );
    const auto xdgSize = strlen( xdg );

    // -1 - \0 on HYPR and SOCK
    std::memcpy( addr.sun_path, xdg, xdgSize );
    std::memcpy( addr.sun_path + xdgSize, HYPR, sizeof(HYPR) );
    std::memcpy( addr.sun_path + xdgSize + sizeof(HYPR) - 1, sign, signSize );
    std::memcpy( addr.sun_path + xdgSize + sizeof(HYPR) - 1 + signSize, SOCK, sizeof(SOCK) );

   /* if( connect(_sock, reinterpret_cast<sockaddr*>( &addr ), sizeof( addr ) ) ) {
        logger.log(LogLvl::Error, "Internal. Failed to connect to Hyprland socket. Path: ", addr.sun_path );
    } */
}

_Hyprland::~_Hyprland() {
    logger.log(LogLvl::Info, "Hyprland socket closed");
    // close( _sock );
}

void _Hyprland::castToBase() {
    this->~_Hyprland();

    new( this ) DesktopEnv;
}

ProcessInfo _Hyprland::getFocused() {
    if(( _sock = socket( AF_UNIX, SOCK_STREAM, 0 ) ) < 0 ) {
        logger.log(LogLvl::Error, "Internal. Failed to create fd for socket");
    }

    if( connect(_sock, reinterpret_cast<sockaddr*>( &addr ), sizeof( addr ) ) ) {
        logger.log(LogLvl::Error, "Internal. Failed to connect to Hyprland socket. Path: ", addr.sun_path );
    }

    ProcessInfo result;
    if( send( _sock, MSG, sizeof(MSG), 0 ) < 0 ) {
        logger.log(LogLvl::Warning, "Unable to send to hyprland. Rollback...");
        checkDE();
        return {};
    }

    // God damn, it is csv or some like
    std::string buffer( INIT_SIZE, '\0' );

    
    int rc = read( _sock, buffer.data(), buffer.size() );

    // rc == 0 <-> EOF
    while( rc != 0 ) {
        if( rc < 0 ) {
            logger.log(LogLvl::Error, "Internal. Unable to read from socket");
        }

        auto size = buffer.size();
        buffer.resize( buffer.size() * 2 );
        // we read size, not buffer size coz of shift
        rc = read( _sock, buffer.data() + size, size );
    }

    // ----------------- class (name)------------------- //
    auto it = std::find( buffer.begin(), buffer.end(), '\n' );
    ++it;
    for( int i = 1; i < 9; ++i ) {
        it = std::find( it, buffer.end(), '\n' );
        ++it;
    }

    // i can't iterate by ':' because it can be in class, title, etc
    it = std::find( it, buffer.end(), ':' );
    it += 2;

    auto end = std::find( it, buffer.end(), '\n' );
    if( it >= end ) return {};
    result.name = { it, end };
    // std::strncpy( result.name.data(), it.base(),
    //        std::min( result.name.size(), (unsigned long)(end - it) ) );


    // --------------- title (descriton) --------------- //
    it = std::find( end, buffer.end(), ':' );
    it += 2;
    
    end = std::find( it, buffer.end(), '\n' );
    result.describe = { it, end };

    // ------------------------ Uptime ------------------- //
    for( int i = 0; i < 3; ++i ) {
        it = std::find( it, buffer.end(), '\n' );
        ++it;
    }

    it = std::find( it, buffer.end(), ':' );
    it += 2;

    end = std::find( it, buffer.end(), '\n' );


    std::array<char, 6> pid{};
    std::memcpy( pid.data(), it.base(), end - it );
    // result.uptime = ps( pid );

    logger.log(LogLvl::Info, "tueried data: ", result);
    close( _sock );

    return result;
}

bool _Hyprland::CastCondition() {
    logger.log(LogLvl::Info, "Checking does Hyprland running...");
    char* de( blockingGetEnv( DE_ENV_VAR ) );

    if( !de ) {
        logger.log(LogLvl::Error, "Unable to detect current DE!");
        throw std::runtime_error("Unable to detect current DE!");
    }

    return strstr( "Hyprland", de ) != nullptr;
}

void _Hyprland::InplaceCast( DesktopEnv* self ) {
    self->~DesktopEnv();

    new( self ) _Hyprland;
}
