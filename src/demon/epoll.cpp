#include <demon/epoll.hpp>
#include <common/logger.hpp>

#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

static int epoll_fd;

Epoll::Epoll( Modules modules ) {
    _fd = epoll_create( 52 ); // arg is deprecated and do nothings
    if( _fd == -1 ) {
        logger.log( LogLvl::Error, "Unable to create epoll: ", strerror( errno ) );
        throw std::runtime_error("Uable to create epoll");
    }

    epoll_fd = _fd;
    ready.resize( 16 );

    epoll_event ev;
    for( const auto& module : modules ) {
        if( module.first != -1 ) {
            logger.log(LogLvl::Info, "Module in listen!");
            ev.data.fd = module.first;
            ev.events = EPOLLIN;

            // potential FIXME. Should it be created in heap?
            if( epoll_ctl( _fd, EPOLL_CTL_ADD, module.first, &ev ) == -1 ) [[unlikely]] {
                logger.log(LogLvl::Error, "Unable to add an event to epoll: ", strerror( errno ));
                std::runtime_error("Unable to add an event to epoll!");
            }
        }
    } 
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

// lazy solve. should pass epoll fd in module guess
void Subscribe( int fd ) {
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    epoll_ctl( epoll_fd, EPOLL_CTL_ADD, fd, &ev );
}

void Unsubscribe( int fd ) {
    epoll_ctl( epoll_fd, EPOLL_CTL_DEL, fd, nullptr );

}
