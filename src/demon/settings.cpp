#include "demon/plugin.hpp"
#include <cassert>
#include <cstring>
#include <demon/settings.hpp>

#include <stdexcept>
#include <sys/inotify.h>

static const uint32_t MASK = IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF;

static void init( int fd, std::string_view path ) {
    fd = inotify_init1( IN_NONBLOCK | IN_CLOEXEC );

    inotify_add_watch( fd, path.data(), MASK );
}

NotifySettings::NotifySettings() : Plugin( true ) {
    init( getFd(), _path.c_str() ); // why c_str? Coz, guess, string return new allocated string

    inotify_event events[8]; // YAY, magic number ^^. // most of editors generate 3-4 events per file change

    long n = read( getFd(), events, sizeof( events ) );

    if( n == -1 ) {
        logger.log(LogLvl::Error, "Unable to read from inotify! ", strerror(errno));
        throw std::runtime_error( "Unable to read from inotify!" );
    }

    assert( n % sizeof( inotify_event ) == 0 );
    int count = n / sizeof( inotify_event );
    logger.log(LogLvl::Info, "Inotify send ", count, " events!");

    if( count != 1 || (events[0].mask & IN_MODIFY) == 0 ) {
        inotify_add_watch( getFd(), _path.c_str(), MASK );
    }

    scan();
}

