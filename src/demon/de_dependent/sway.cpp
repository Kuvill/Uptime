#include "demon/better_uptime.hpp"
#include "common/logger.hpp"
#include "common/aliases.hpp"
#include "demon/epoll.hpp"

#include <cstdio>
#include <cstring>

#include <stdexcept>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include <cassert>
#include <stack>
#include <array>

static const char* DE_ENV_VAR = "XDG_CURRENT_DESKTOP";

// have to give it machine to interpret them in native endian
static const std::byte WorkspacesQuerry[] = {
    std::byte('i'), std::byte('3'), std::byte('-'), // i3-
    std::byte('i'), std::byte('p'), std::byte('c'), // ipc
    std::byte(0x0A), std::byte(0), std::byte(0), std::byte(0), // 10 - msg len
    std::byte(2), std::byte(0), std::byte(0), std::byte(0), // 2 - subscribe
    std::byte('['), std::byte('"'), // json with len 10: event on window prop chnage
    std::byte('w'), std::byte('i'), std::byte('n'),
    std::byte('d'), std::byte('o'), std::byte('w'),
    std::byte('"'), std::byte(']'),
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

std::string _SwayDE::getAnswer() {
    std::string data;

    std::array<char, 14> msgSize;
    int rc = send(_fd, WorkspacesQuerry, sizeof(WorkspacesQuerry), 0);

    rc = read( _fd, msgSize.data(), sizeof( msgSize ) );
    if( rc < 0 ) {
        logger.log(LogLvl::Error, "Failed to get whole message from sway!");
        throw std::runtime_error("Failed to get whole message from sway!");
    }

    const uint32_t size = *reinterpret_cast<uint32_t*>( msgSize.data()+6 );

    data.resize( size );

    rc = read( _fd, data.data(), size );

    return data;
}

_SwayDE::_SwayDE() {
    logger.log(LogLvl::Info, "Sway detected!");
    if(( _fd = socket( AF_UNIX, SOCK_STREAM, 0 ) ); _fd < 0 ) {
        logger.log(LogLvl::Error, "Unable to create socket!!");
        throw std::runtime_error("Unable to create socket!!");
        // get sockerr
    }

    Subscribe( _fd, this );

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    const char* sockAddr = getSwaySockAddr();

    if( !sockAddr ) [[unlikely]] {
        c_string trueAddr { getSwaySockAddrAlter() };
        sockAddr = trueAddr.get();

        if( !trueAddr ) {
            logger.log(LogLvl::Error, "With seted session env on sway, sockadr wasn't found!");
            throw std::runtime_error("With seted session env on sway, sockadr wasn't found!");
        }
    }

    strcat( addr.sun_path, sockAddr );

    if( connect(_fd, reinterpret_cast<sockaddr*>( &addr ), sizeof(addr) ) < 0 ) {
        logger.log(LogLvl::Error, "Unable to connect to sway socket! ", sockAddr);
        throw std::runtime_error("Unable to connect to sway socket!");
    }

    int rc = send(_fd, WorkspacesQuerry, sizeof(WorkspacesQuerry), 0);
    if( rc < 0 ) {
        logger.log(LogLvl::Warning, "Probably, sway socket has been closed. Unable to send message");
        checkDE();
    }

    nlohmann::json answer = nlohmann::json::parse (getAnswer() );
    logger.log( LogLvl::Info, answer );
    if( answer["success"] == false ) {
        logger.log(LogLvl::Error, "Unable to subscribe to an sway event!");
        throw std::runtime_error( "Unable to subscribe to an sway event!" );
    }

    logger.log(LogLvl::Info, "Subscribed to sway event!");
}

_SwayDE::~_SwayDE() {
    logger.log(LogLvl::Info, "Sway desctructor called");
}

static ProcessInfo ParseSwayJson( const nlohmann::json& json ) {
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

ProcessInfo _SwayDE::getFocused() {
    ProcessInfo result;
    std::array<char, 14> msgSize;

    int rc = send(_fd, WorkspacesQuerry, sizeof(WorkspacesQuerry), 0);
    if( rc < 0 ) {
        // btw i should check errno FIXME
        logger.log(LogLvl::Warning, "Probably, sway socket has been closed. Unable to send message");

        // if i want call here getFocused, i have to prevent inf loop.
        // IMHO i don't have to do that coz it take time to select new DE
        // (1 more reason why i should call sleep not it begin of program, but after getenv == nullptr)
        checkDE();
        return {};
    }

    rc = read( _fd, msgSize.data(), sizeof( msgSize ) );
    if( rc < 0 )
        logger.log(LogLvl::Error, "Internal. Failed to read message");

    const uint32_t size = *reinterpret_cast<uint32_t*>( msgSize.data()+6 );
    // const uint32_t type = *reinterpret_cast<uint32_t*>( msgSize.data()+10 );

    // string don't allow non-init creation (just use raw char*? or create from it)
    std::string data( size, '\0' );

    rc = read( _fd, data.data(), size );

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
