#include "demon/plugin.hpp"
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

[[maybe_unused]] constexpr const std::string_view CONF_PATH = ".config/uptimer/";
[[maybe_unused]] constexpr const std::string_view CACHE_PATH = ".cache/uptimer/";
[[maybe_unused]] constexpr const std::string_view SHARE_PATH = ".local/share/uptimer/";
[[maybe_unused]] constexpr const std::string_view STATE_PATH = ".local/state/uptimer/";

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

// reorginize with goto as it was firstly?
// not it really hard to add result string conditions:
// '/' at the end
// ~/ at the begin
// windows '\'...
std::string Settings::setupThePath( std::string_view variable, std::string_view def ) {
    auto result = _config["paths"][variable].value<std::string>();
    if( result ) {
        if( adaptPath( result.value() ) == PathErrCode::Good ) {
            if( result.value().back() != '/' ) result.value() += '/';

            return result.value();
        }
    }

    PathErrCode code;
    if( code = adaptPath( def ); code == PathErrCode::Good )
        return std::string(def);

    logger.log( LogLvl::Error, "Unable to configure config path! ", toString( code ) );
    throw std::runtime_error("Unable to configure config path!");
}

Settings::Settings( std::string_view overridenConfPath ) : Plugin( true ) {
    paths.config = overridenConfPath.empty() ?
                    CONF_PATH : overridenConfPath;

    if( !fs::exists(paths.config) ) {
        if( !fs::create_directories( paths.config ) )
            throw std::runtime_error("Unable to open/create config directory!");
    }

    int fd =  inotify_init1( IN_NONBLOCK );
    if( fd == -1 ) {
        logger.log(LogLvl::Error, "Unable create inotify ", strerror(errno));
    }

    setFd( fd );

    reload();
}

void Settings::setupPaths() {
    paths.db = setupThePath( "db", SHARE_PATH );
    paths.log = setupThePath( "log", STATE_PATH );
    paths.lock = setupThePath( "lock", STATE_PATH );
    paths.socket = setupThePath( "socket", SHARE_PATH );
    paths.resources = setupThePath( "resources", SHARE_PATH );

    // yay, magic number ^^. it is sockMaxLen (108) - fileName (12)
    // Yes, i do not check it availabllity. Unlucky
    if( paths.socket.string().size() > 97 ) 
        paths.socket.string() = SHARE_PATH;
}

// before reading new file, i can save old as tmp
void Settings::reload() {
    _config = toml::parse_file( paths.config.string() );

    cd = seconds( _config["general"]["cd"].value_or( 5 ) );

    setupPaths();
}

void Settings::OnTrigger() {
    reload();
}
