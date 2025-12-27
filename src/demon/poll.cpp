#include "demon/poll.hpp"
#include "common/logger.hpp"

#include <cstring>
#include <sys/socket.h>
#include <cerrno>
#include <stdexcept>
#include <unistd.h>

void clearSocket( int fd ) {
    char buf[1024];
    int r = read( fd, buf, 1024 );

    // if message is bigger, then 1024
    if( r == 1024 ) [[unlikely]]
        clearSocket( fd );

    // 0 - eof, >0 - readed all from socket
    if( r != -1 ) [[likely]]
        return;

    // nothings in scoket + nonblock
    if( (errno == EAGAIN || errno == EWOULDBLOCK ) )
        return;

    logger.log( LogLvl::Error, "error after clearing socket! ", strerror(errno) );
    throw std::runtime_error("error after clearing socket!");
}
