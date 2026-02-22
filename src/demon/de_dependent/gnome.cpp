#include "common/logger.hpp"
#include "common/time.hpp"
#include "demon/better_uptime.hpp"

// i hate GIO. Why, just why they hide native fd?
#include <memory>
#include <nlohmann/json.hpp>
#include <sdbus-c++/IConnection.h>
#include <sdbus-c++/IProxy.h>
#include <sdbus-c++/Types.h>
#include <sdbus-c++/sdbus-c++.h>

#include <stdexcept>

#include <unistd.h>
#include <sys/prctl.h>

static const char* DE_ENV_VAR = "XDG_CURRENT_DESKTOP";

void _Gnome::TryStartExtension() {
    // Love dbus - allocate same string 3 times
    sdbus::ServiceName servName{ "org.gnome.shell.extensions.FocusedWindow" };
    sdbus::ObjectPath objPath{ "/org/gnome/shell/extensions/FocusedWindow" };

    _connection = sdbus::createSessionBusConnection();
    // _connection->enterEventLoopAsync(); // allow to use it with my poll

    // Oh. We not connect to an signal, Idk what we do
    // have to be like:
    // std::string match = "type='signal',destination='org.gnome.Shell',path='/org/gnome/shell/extensions/FocusedWindow'";
    // _connection->addMatch(match); // API name may be addMatch or addMatchRule
    /*
    _connection->registerMessageHandler(
        [this](sdbus::Message& msg) {
            // inspect message: type, interface, member (signal name)
            auto type = msg.getMessageType();
            if (type == sdbus::Message::Type::Signal) {
                auto iface = msg.getInterface();
                auto member = msg.getMember();
                // read arguments as needed
                dbusCallback(std::move(msg)); // or handle inline
            }
        }
    );
    */

    auto proxy = sdbus::createProxy( *_connection, std::move(servName), std::move(objPath) );
    sdbus::InterfaceName interface{ "org.gnome.shell.extensions.FocusedWindow" };
    sdbus::SignalName methodName{ "FocusChanged" };
    proxy->registerSignalHandler( interface, methodName, 
            [this]( sdbus::Signal signal ){ dbusCallback( signal ); }
    );

    renewPollData();
}

void _Gnome::dbusCallback( sdbus::Signal signal ) {
    logger.log(LogLvl::Info, "Gnome dbus callback triggered!");
    std::string rawData;
    signal >> rawData;

    auto json = nlohmann::json::parse( std::move(rawData) );

    ProcessInfo info;
    info.name = json["wm_class"];
    info.describe = json["title"];
    info.timestomp = getCurrentTime();

    _processInfo = std::make_unique<ProcessInfo>( info );
}

void _Gnome::renewPollData() {
    auto poll_info = _connection->getEventLoopPollData();
    _fd = poll_info.fd;
}

_Gnome::_Gnome() {
    logger.log(LogLvl::Info, "Gnome detected!");
    TryStartExtension();

    if( _fd == -1 ) {
        logger.log(LogLvl::Error, "An Error occured while start gnome dbus listener! ");
        throw std::runtime_error("An Error occured while start gnome dbus listener!");
    }
}

ProcessInfo _Gnome::getFocused() {
    _connection->processPendingEvent();
    renewPollData();

    return *_processInfo;
}

bool _Gnome::CastCondition() {
    logger.log(LogLvl::Info, "Checking does sway running...");

    char* de( blockingGetEnv( DE_ENV_VAR ) );

    if( !de ) {
        logger.log(LogLvl::Error, "Unable to detect current DE!");
        throw std::runtime_error("Unable to detect current DE!");
    }

    return strstr( de, "GNOME" ) != nullptr;
}

void _Gnome::InplaceCast( DesktopEnv *self ) {
    self->~DesktopEnv();

    new( self ) _Gnome;
}
