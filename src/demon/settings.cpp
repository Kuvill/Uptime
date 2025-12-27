#include <demon/settings.hpp>

#include "common/logger.hpp"
#include <cassert>
#include <cstring>

#include <stdexcept>
#include <sys/inotify.h>

static const uint32_t MASK = IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF;

static void init( int fd, std::string_view path ) {
    fd = inotify_init1( IN_NONBLOCK | IN_CLOEXEC );

    inotify_add_watch( fd, path.data(), MASK );
}

NotifySettings::NotifySettings() {
    init( _fd, _path.c_str() ); // why c_str? Coz, guess, string return new allocated string

    scan();
}

void NotifySettings::OnTrigger() {
    inotify_event events[8]; // YAY, magic number ^^. // most of editors generate 3-4 events per file change

    long n = read( _fd, events, sizeof( events ) );

    if( n == -1 ) {
        logger.log(LogLvl::Error, "Unable to read from inotify! ", strerror(errno));
        throw std::runtime_error( "Unable to read from inotify!" );
    }

    assert( n % sizeof( inotify_event ) == 0 );
    int count = n / sizeof( inotify_event );
    logger.log(LogLvl::Info, "Inotify send ", count, " events!");

    if( count != 1 || (events[0].mask & IN_MODIFY) == 0 ) {
        inotify_add_watch( _fd, _path.c_str(), MASK );
    }

}

NotifySettings::NotifySettings( std::string_view path ) : NotifySettings() {
    _path = path;
}

NotifySettings::~NotifySettings() = default;
