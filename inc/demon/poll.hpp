#pragma once

#include <sys/poll.h>
#include <array>

// holy, i HAVE to use enum over enum class to be able: 
//  1. use lazy | and &
//  2. to cast to boolean implictly
// Btw, it is right since it is not close set
enum PollEvent : short {
    In = POLLIN,
    Closed = POLLHUP,
    Err = POLLHUP,
};

struct safePFD {
    int fd;
    PollEvent event;
    short _{};

    operator pollfd() const {
        return pollfd( fd, short(event), _ );
    }
};

struct PfdWithId {
    safePFD pfd;   
    int* id;
};

static_assert(sizeof(safePFD) == sizeof(pollfd), "Invalid safePFD helper class");

template< std::size_t _size >
class Poll {
    std::array<pollfd, _size> _fds;

public:
    Poll( std::initializer_list<PfdWithId> fds ) {
        int i = 0;
        for( auto& fd : fds) {
            _fds[i] = fd.pfd;
            *fd.id = i;

            ++i;
        }
    }
    
    int wait() {
       int result = poll( _fds.data(), _size, -1 );

       if( result == -1 ) 
           throw "";

       return result;
    }

    pollfd operator[]( std::size_t index ) {
        return _fds[index];
    }

    consteval std::size_t size() {
        return _size;
    }
};

void clearSocket( int fd );
