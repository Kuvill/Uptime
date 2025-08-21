#include "demon/better_uptime.hpp"
#include "common/logger.hpp"

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
}

// constexpr ...(
void registrateAll() {
    registrate<_SwayDE>();
    registrate<_Hyprland>();
}

size_t sizeForDE() {
    size_t result = 0;

    for( auto size : __Sizes__ ) {
        result = std::max( result, size );
    }

    return result;
}

// change strstr to iterating by ':' as in sway:wlroot:swayfx 
// allow function itself to call getenv? Can't see why not
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
    logger.log(LogLvl::Warning, "Current Desktop enviroment didn't recognized!");
    return {};
}

void DesktopEnv::castToBase() {}
