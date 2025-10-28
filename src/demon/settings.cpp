#include <demon/settings.hpp>

#include <filesystem>
#include <optional>
#include <string_view>

#include <toml++/toml.hpp>

#include <cstring>
#include <sys/inotify.h>

using namespace std::chrono;
using namespace std::chrono_literals;

namespace fs = std::filesystem;

constexpr const std::string_view CONF_PATH = ".config/uptimer/";
constexpr const std::string_view CACHE_PATH = ".cache/uptimer/";
constexpr const std::string_view SHARE_PATH = ".local/share/uptimer/";
constexpr const std::string_view STATE_PATH = ".local/state/uptimer/";

enum class PathErrCode : char {
    Good,
    CreateErr,
    ModErr
};

constexpr std::string_view toString( PathErrCode code ) {
    switch( code ) {
        case PathErrCode::CreateErr:
            return "Unable to create path";
            break;

        case PathErrCode::ModErr:
            return "Wrong file/directory premissions";
            break;

        case PathErrCode::Good:
            throw std::runtime_error("Success path passed into error handler!");
    }
}

// May create and chmod
static PathErrCode adaptPath( std::string_view path ) {
    if( !fs::exists( path ) )
        if( !fs::create_directories( path ) )
            return PathErrCode::CreateErr;

    auto stat = fs::status( path );
    if( (stat.permissions() & fs::perms::owner_read) == fs::perms::none ||
            (stat.permissions() & fs::perms::owner_write) == fs::perms::none ) {

        return PathErrCode::ModErr;
    }

    return PathErrCode::Good;
}

std::string Settings::setupThePath( std::string_view variable, std::string_view def ) {
    auto result = _config["paths"][variable].value<std::string>();
    if( result ) {
        if( adaptPath( result.value() ) == PathErrCode::Good )
            return result.value();
    }

    PathErrCode code;
    if( code = adaptPath( def ); code == PathErrCode::Good )
        return std::string(def);

    logger.log( LogLvl::Error, "Unable to configure config path! ", toString( code ) );
    throw std::runtime_error("Unable to configure config path!");
}

Settings::Settings( std::string_view overridenConfPath ) {
    paths.config = overridenConfPath.empty() ?
                    CONF_PATH : overridenConfPath;

    int fd =  inotify_init1( IN_NONBLOCK );
    if( fd == -1 ) {
        logger.log(LogLvl::Error, "Unable create inotify ", strerror(errno));
    }

    setFd( fd );

    reload();
}

void Settings::setupPaths() {
    setupThePath( "cache", CACHE_PATH );
    setupThePath( "share", SHARE_PATH );
    setupThePath( "state", STATE_PATH );
}

// before reading new file, i can save old as tmp
void Settings::reload() {
    _config = toml::parse_file( paths.config.string() );

    cd = seconds( _config["general"]["cd"].value_or( 5 ) );
    logger.log(LogLvl::Info, "Seconds: ", cd);

    setupPaths();
}

void Settings::OnTrigger() {
    reload();
}
