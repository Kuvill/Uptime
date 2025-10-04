#include <demon/epoll.hpp>
#include <common/logger.hpp>
#include <demon/plugin.hpp>

#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

static int epoll_fd;

Epoll::Epoll() {
    _fd = epoll_create( 52 ); // arg is deprecated and do nothings
    if( _fd == -1 ) {
        logger.log( LogLvl::Error, "Unable to create epoll: ", strerror( errno ) );
        throw std::runtime_error("Uable to create epoll");
    }

    epoll_fd = _fd;
    ready.resize( 16 );
}

Epoll::~Epoll() {
    close( _fd );
}

int Epoll::wait() {
    int result = epoll_wait(_fd, ready.data(), ready.size(), -1 );

    if( result == -1 ) [[unlikely]] {
        logger.log(LogLvl::Error, "Unable to call epoll_wait: ", strerror( errno ));
        throw std::runtime_error("Unable to call epoll_wait");
    }

    return result;
}

// i don't like idea to place epoll_fd in global space
void Subscribe( int fd, Plugin* plugin ) {
    epoll_event ev;
    ev.data.ptr = plugin;
    ev.events = EPOLLIN;
    epoll_ctl( epoll_fd, EPOLL_CTL_ADD, fd, &ev );
    logger.log(LogLvl::Info, "Registrate class: ", typeid(*plugin).name());
}

void Unsubscribe( int fd ) {
    epoll_ctl( epoll_fd, EPOLL_CTL_DEL, fd, nullptr );
    logger.log(LogLvl::Info, "Unregistrate class");

}
