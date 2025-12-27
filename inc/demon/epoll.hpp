#pragma once

#include <sys/poll.h>
#include <array>
#include <algorithm>

/*
class Epoll {
public:
    std::vector<epoll_event> ready; // btw it shouldn't be vector
private:
    sigset_t ss; // i have to use pwait to prevent UB on signal catch
    int _fd;

public:
    Epoll();

    ~Epoll();

    // blocking. Error checked
    int wait();
};

void Subscribe( int fd, Plugin* );
void Unsubscribe( int fd );
*/

// short to be same type as pollfd
enum class PollEvent : short {
    In = POLLIN,
    Err = POLLHUP
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

    pollfd operator[]( size_t index ) {
        return _fds[index];
    }

    consteval size_t size() {
        return _size;
    }
};

