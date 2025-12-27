#include "common/time.hpp"
#include "demon/better_uptime.hpp"
#include "common/logger.hpp"
#include "common/aliases.hpp"
#include "demon/poll.hpp"

#include <chrono>
#include <cstdio>
#include <cstring>

#include <stdexcept>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include <cassert>
#include <array>

using namespace std::chrono_literals;

static const char* DE_ENV_VAR = "XDG_CURRENT_DESKTOP";

using namespace std::chrono_literals;

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

    int sum = 0;
    int rc = read( _fd, msgSize.data() + sum, sizeof( msgSize ) - sum );

    while( true ) {
        logger.log(LogLvl::Info, rc);
        if( rc + sum == 14 ) break;

        if( rc < 0 ) {
            if( errno != EAGAIN && errno != EWOULDBLOCK ) {
                logger.log(LogLvl::Error, "Failed to get whole message from sway! ", strerror(errno));
                perror("read");
                throw std::runtime_error("Failed to get whole message from sway!");

            } else 
                rc = 0;
        }

        sum += rc;
        std::this_thread::sleep_for( 1ms ); // mb add some guard... (36000 ns while spin lock + stdout)

        rc = read( _fd, msgSize.data(), sizeof( msgSize ) );
    }

    // first 6 - magic string. next 4 - size, 4 - type
    const uint32_t size = *reinterpret_cast<uint32_t*>( msgSize.data()+6 );

    data.resize( size );

    rc = read( _fd, data.data(), size );

    logger.log(LogLvl::Warning, msgSize.data() );
    logger.log(LogLvl::Warning, data );

    return data;
}

_SwayDE::_SwayDE() {
    logger.log(LogLvl::Info, "Sway detected!");
    if(( _fd = socket( AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0 ) {
        logger.log(LogLvl::Error, "Unable to create socket!! ", strerror(errno));
        throw std::runtime_error("Unable to create socket!!");
    }

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

    nlohmann::json answer = nlohmann::json::parse( getAnswer() );


    if( answer["success"] == false ) {
        logger.log(LogLvl::Error, "Unable to subscribe to an sway event!");
        throw std::runtime_error( "Unable to subscribe to an sway event!" );
    }

    logger.log(LogLvl::Info, "Subscribed to sway event!");
}

_SwayDE::~_SwayDE() {
    close( _fd );
    logger.log(LogLvl::Info, "Sway desctructor called");
}

static ProcessInfo ParseSwayJson( const nlohmann::json& json ) {
    ProcessInfo result;

    if( std::string msg = json["change"]; msg != "focus" && msg != "new" ) {
        logger.log(LogLvl::Info, "Handled useless sway event: ", msg, ". Skiping...");
        return {};
    }

    auto node = json.find("container");

    if( node != json.end() ) {
        auto name = node->find("app_id");
        (name != node->end() && !name->is_null()) ?
            void(result.name = name->get<std::string>()) :
            logger.log(LogLvl::Error, "Unexpected sway answer! App name wasn't found!");

        auto describe = node->find("name");
        (describe != node->end() && !describe->is_null()) ?
            void(result.describe = describe->get<std::string>()) :
            logger.log(LogLvl::Error, "Unexpected sway answer! App describe wasn't found!");
    }

    return result;
}

ProcessInfo _SwayDE::getFocused() {
    ProcessInfo result;

    nlohmann::json json = nlohmann::json::parse( getAnswer() );
    result = ParseSwayJson( json );
    result.timestomp = getCurrentTime();

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
