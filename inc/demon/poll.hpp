#pragma once

#include "common/logger.hpp"
#include "demon/modules.hpp"
#include <algorithm>
#include <array>
#include <bits/types/sigset_t.h>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <cstddef>
#include <cassert>

#include <stdexcept>
#include <sys/eventfd.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using Callback = void(*)( int, uint32_t );

enum class PollEvent : short {
    In = POLLIN,
    Err = POLLHUP
};

struct safePFD {
    int fd;
    PollEvent event;

    operator pollfd() const {
        return pollfd( fd, short(event) );
    }
};

template< size_t _size >
class Poll {
    std::array<pollfd, _size> _fds;

public:
    Poll( std::initializer_list<safePFD> fds ) {
        std::copy_n(fds.begin(), fds.size(), _fds.begin());
    }
    
    // mb needed froze function

    int listen() {
       return poll( _fds.data(), _size, -1 );
    }

    pollfd operator[]( size_t index ) {
        return _fds[index];
    }

    consteval size_t size() {
        return _size;
    }
};

class Epoll {
public:
    std::vector<epoll_event> ready;
private:
    sigset_t ss; // i have to use pwait to prevent UB on signal catch
    int _fd;

public:
    Epoll( Modules modules  ) {
        _fd = epoll_create( 52 ); // arg is deprecated and do nothings
        if( _fd == -1 ) {
            logger.log( LogLvl::Error, "Unable to create epoll: ", strerror( errno ) );
            throw std::runtime_error("Uable to create epoll");
        }

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

    ~Epoll() {
        close( _fd );
    }

    // blocking. Error checked
    int wait() {
        int result = epoll_wait(_fd, _ready.data(), _ready.size(), -1 );

        if( result == -1 ) [[unlikely]] {
            logger.log(LogLvl::Error, "Unable to call epoll_wait: ", strerror( errno ));
            throw std::runtime_error("Unable to call epoll_wait");
        }

        return result;
    }
};
