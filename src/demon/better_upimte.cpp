#include "demon/better_uptime.hpp"
#include "common/logger.hpp"

#include <memory>
#include <typeinfo>
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

// YOO, SOLID, i recived you ^^
// Task for fun (not to impl): Make it dynamic without recompile

// coast of SOLID - really big. not sure, that it was relevant

// Can it be std::tuple to avoid allocation and make all constexpr ?)
// alter way - use vectors. (create it with lambda)
// +: clear it to call destructors; space

// List of supported DE. Change it to add custom 
// Order make a sence

namespace {
    static auto __De_instances__ =
        std::to_array<std::unique_ptr<DesktopEnv>>({
           
            std::make_unique<_SwayDE>(),
            std::make_unique<_Hyprland>()
    });

    // auto + -> {type} look better, but harder to get, that it not a lamda
    static std::array<CastRule, std::size(__De_instances__)> __Rules__ = []() constexpr {
        std::array<CastRule, std::size(__De_instances__)> result;

        for( int i = 0; i < __De_instances__.size(); ++i ) {
            result[i] = __De_instances__[i]->returnCastRule();
        }

        return result;
    }();
}

ulong DE_maxSize() {
    unsigned long result = 0;

    logger.log(LogLvl::Info, "Supported Desktop enviroments: ");

    for( const auto& DE : __De_instances__ ) {
        result = std::max( DE->getSizeof(), result );
        logger.log(LogLvl::Info, '\t', typeid(DE).name() );
    }

    return result;
} 

// change strstr to iterating by ':' as in sway:wlroot:swayfx 
// allow function itself to call getenv? Can't see why not
DesktopEnv* DesktopEnv::checkDE() {
    logger.log(LogLvl::Info, "Recheck current DE");
    char* de( std::getenv( DE_ENV_VAR ) );

    for( int i = 0; i < __Rules__.size(); ++i ) {
        if( __Rules__[i].cond( de ) ) {
            __Rules__[i].cast( this );
        }
    }

    if( !de ) {
        logger.log(LogLvl::Error, "Unable to detect current DE!");
        throw std::runtime_error("Unable to detect current DE!");
    }

    else return this;
}

// I don't recall getFocused to prevent inf recursive
ProcessInfo DesktopEnv::getFocused() {
    logger.log(LogLvl::Warning, "Current Desktop enviroment didn't recognized!");
    return {};
}

void DesktopEnv::castToBase() {}

CastRule DesktopEnv::returnCastRule() const {
    throw std::runtime_error("Internal. Base DE class passed to registering");
}

size_t DesktopEnv::getSizeof() {
    throw std::runtime_error("Internal. Base DE class passed to registering");
}
