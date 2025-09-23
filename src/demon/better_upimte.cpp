#include "demon/better_uptime.hpp"
#include "common/logger.hpp"
#include "demon/epoll.hpp"

#include <algorithm>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// !!!!! FIXME
// on send to closed socket will be called SIGPIPE. i should catch it or ignore
// (second will be faster guess)
// MSG_NOSIGNAL

#include <nlohmann/json.hpp>


// YOO, SOLID, i recived you ^^
// Task for fun (not to impl): Make it dynamic without recompile

// Can it be std::tuple to avoid allocation and make all constexpr ?)
// alter way - use vectors. (create it with lambda)
// +: clear it to call destructors; space

// List of supported DE. Change it to add custom 
// Order make a sence

static std::vector<size_t> __Sizes__;
static std::vector<CastRule> __Rules__;

template< typename T >
concept CompleteDE = requires( DesktopEnv* self ) { 
    std::derived_from<T, DesktopEnv>;

    { T::CastCondition() } -> std::same_as<bool>;
    { T::InplaceCast( self ) } -> std::same_as<void>;
};

template< CompleteDE T >
constexpr static void registrate() {
    __Sizes__.push_back( sizeof(T) );
    __Rules__.push_back( CastRule{T::CastCondition, T::InplaceCast} );

    logger.log(LogLvl::Info, "Desktop enviroment class registrated: ", typeid(T).name() );
}

// constexpr ...(
// should be in separated header
void registrateAll() {
    registrate<_SwayDE>();
    registrate<_Hyprland>();
}

size_t sizeForDE() {
    return *std::max_element( __Sizes__.begin(), __Sizes__.end() );
}

DesktopEnv* initDE() {
    registrateAll();
    void* reserve = malloc(sizeForDE());
    DesktopEnv* de = new(reserve) DesktopEnv; 
    de->checkDE();
    
    return de;
}

void DesktopEnv::checkDE() {
    logger.log(LogLvl::Info, "Recheck current DE");

    for( int i = 0; i < __Rules__.size(); ++i ) {
        if( __Rules__[i].cond() ) {
            __Rules__[i].cast( this );
            return;
        }
    }
}

// I don't recall getFocused to prevent inf recursive
ProcessInfo DesktopEnv::getFocused() {
    checkDE();
    logger.log(LogLvl::Warning, "Current Desktop enviroment didn't recognized!");
    return {};
}

void DesktopEnv::castToBase() {
    this->~DesktopEnv(); // since destructor virtual - all good

    new( this ) DesktopEnv;
    _sock = -1;
}

DesktopEnv::~DesktopEnv() {
    logger.log(LogLvl::Info, "Close socket");

    Unsubscribe( _sock );
    close( _sock );
}

