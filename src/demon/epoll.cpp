#include <demon/epoll.hpp>
#include <common/logger.hpp>

#include <unistd.h>
#include <cstring>

Epoll::Epoll( Modules modules ) {
    _fd = epoll_create( 52 ); // arg is deprecated and do nothings
    if( _fd == -1 ) {
        logger.log( LogLvl::Error, "Unable to create epoll: ", strerror( errno ) );
        throw std::runtime_error("Uable to create epoll");
    }

    ready.resize( 16 );

    epoll_event ev;
    for( const auto& module : modules ) {
        ev.data.fd = module.first;
        ev.events = EPOLLIN;

        // potential FIXME. Should it be created in heap?
        if( epoll_ctl( _fd, EPOLL_CTL_ADD, module.first, &ev ) == -1 ) [[unlikely]] {
            logger.log(LogLvl::Error, "Unable to add an event to epoll: ", strerror( errno ));
            std::runtime_error("Unable to add an event to epoll!");
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
