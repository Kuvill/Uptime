#pragma once

#include "demon/plugin.hpp"
#include <sys/epoll.h>
#include <vector>

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

/*

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

*/
