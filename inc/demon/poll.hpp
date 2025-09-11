#pragma once

#include <algorithm>
#include <array>
#include <initializer_list>
#include <cstddef>
#include <cassert>

#include <sys/eventfd.h>
#include <sys/poll.h>
#include <unistd.h>

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

class LightEvent {
    int fd;

public:
    LightEvent() {
        fd = eventfd( 0, 0 );
        assert( fd > 0 );
    }

    // btw all those clear things slower turn off -> slower turn off pc
    // but anyway will be cleaned by OS
    ~LightEvent() {
        close( fd );
    }

    operator int() {
        return fd;
    }
};
