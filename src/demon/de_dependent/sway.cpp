#include "demon/better_uptime.hpp"
#include "common/logger.hpp"
#include "common/aliases.hpp"

#include <cstdio>
#include <cstring>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include <cassert>
#include <stack>
#include <array>

static const char* DE_ENV_VAR = "XDG_CURRENT_DESKTOP";

static const std::byte WorkspacesQuerry[] = {
    std::byte(105), std::byte(51), std::byte(45), // i3-
    std::byte(105), std::byte(112), std::byte(99), // ipc
    std::byte(0), std::byte(0), std::byte(0), std::byte(0), // 0
    std::byte(4), std::byte(0), std::byte(0), std::byte(0), // 4
};

static const char* getSwaySockAddr() {
    auto result = blockingGetEnv( "SWAYSOCK" );

    if( result )
        return result;
    return nullptr;
}

// must be free
static const char* getSwaySockAddrAlter() {
    const char* result;
    
    FILE* mimic = popen( "sway --get-socketpath", "r" );
    assert( mimic != nullptr );

    std::array<char, 256> secondTry;
    char* bufPtr = fgets( secondTry.data(), secondTry.size(), mimic );
    if( bufPtr == nullptr ) 
        throw std::runtime_error( "Socket addr is unknown. Have to use old method" );

    result = new char( strlen( secondTry.data() ) );
    return result;
}

_SwayDE::_SwayDE() {
    logger.log(LogLvl::Info, "Sway detected!");
    if(( _sock = socket( AF_UNIX, SOCK_STREAM, 0 ) )) {

    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    const char* sockAddr = getSwaySockAddr();

    if( !sockAddr ) [[unlikely]] {
        c_string trueAddr { getSwaySockAddrAlter() };
        sockAddr = trueAddr.get();

        if( !trueAddr ) {
            logger.log(LogLvl::Error, "With seted session env on sway, sockadr wasn't found!");
            exit(1);
        }
    }

    strcat( addr.sun_path, sockAddr );

    if( connect(_sock, reinterpret_cast<sockaddr*>( &addr ), sizeof(addr) ) < 0 ) {

    }
}

_SwayDE::~_SwayDE() {
    logger.log(LogLvl::Info, "Sway socket closed");
    close( _sock );
}

static ProcessInfo ParseSwayJson( nlohmann::json json ) {
    ProcessInfo result;

    std::stack<const nlohmann::json*> recursive;
    recursive.push( &json );
    while( !recursive.empty() ) {
        auto& node = *recursive.top();
        recursive.pop();

        if( node.find("pid") != node.end() ) {

            if( node["focused"] ) {
                result.name = node["app_id"];
                result.describe = node["name"];
                // result.uptime = ps( std::to_string( node["pid"].get<int>() ) );

            } // plase for recording all data

        } else {
            for( const auto& child : node["nodes"] ) {
                recursive.push( &child );
            }
        }
    }

    return result;
}

// do not ask, i just want it d:
// shoud it be std::start_lifestart_as? (not available in any compiler yet)
void _SwayDE::castToBase() {
    this->~_SwayDE();

    new( this ) DesktopEnv;
}

ProcessInfo _SwayDE::getFocused() {
    ProcessInfo result;
    std::array<char, 14> msgSize;

    int rc = send(_sock, WorkspacesQuerry, sizeof(WorkspacesQuerry), 0);
    if( rc < 0 ) {
        // btw i should check errno FIXME
        logger.log(LogLvl::Warning, "Probably, sway socket has been closed. Unable to send message");

        // if i want call here getFocused, i have to prevent inf loop.
        // IMHO i don't have to do that coz it take time to select new DE
        // (1 more reason why i should call sleep not it begin of program, but after getenv == nullptr)
        checkDE();
        return {};
    }

    rc = read( _sock, msgSize.data(), sizeof( msgSize ) );
    if( rc < 0 )
        logger.log(LogLvl::Error, "Internal. Failed to read message");

    const uint32_t size = *reinterpret_cast<uint32_t*>( msgSize.data()+6 );
    // const uint32_t type = *reinterpret_cast<uint32_t*>( msgSize.data()+10 );

    // string don't allow non-init creation (just use raw char*? or create from it)
    std::string data( size, '\0' );

    rc = read( _sock, data.data(), size );

    if( rc < 0 ) {
        logger.log(LogLvl::Error, "Internal. Failed to read message");
    }

    nlohmann::json json = nlohmann::json::parse( data );

    result = ParseSwayJson( json );

    logger.log(LogLvl::Info, "tueried data: ", result);

    return result;
}

bool _SwayDE::CastCondition() {
    logger.log(LogLvl::Info, "Checking does sway running...");

    char* de( blockingGetEnv( DE_ENV_VAR ) );

    if( !de ) {
        logger.log(LogLvl::Error, "Unable to detect current DE!");
        throw std::runtime_error("Unable to detect current DE!");
    }

    return strstr( de, "sway" ) != nullptr;
}

void _SwayDE::InplaceCast( DesktopEnv* self ) {
    self->~DesktopEnv();

    new( self ) _SwayDE;
} 
