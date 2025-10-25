#include <demon/settings.hpp>
#include <string_view>

#include <cstring>
#include <sys/inotify.h>

Settings::Settings( std::string_view path ) : _path( path ) {
    int fd =  inotify_init1( IN_NONBLOCK );
    if( fd == -1 ) {
        logger.log(LogLvl::Error, "Unable create inotify ", (path), "). ", strerror(errno));
    }

    setFd( fd );

    reload();
}

void Settings::reload() {
    
}
