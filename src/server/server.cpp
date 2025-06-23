#include "server/server.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdlib>
#include "common/logger.hpp"

Server::Server( const int port ) {
    _serverSocket = socket( AF_UNIX, SOCK_STREAM, 0 );
    if( _serverSocket < 0 ) {
        logger.log(LogLvl::Error, "Unable to create server socket!");
        exit(-1);
    }

    sockaddr_un serverAddr{};
    serverAddr.sun_family = AF_UNIX;
    strcpy( serverAddr.sun_path, PATH );

    if( bind(_serverSocket, (sockaddr*)&serverAddr, sizeof( serverAddr )) < 0 ) {
        logger.log(LogLvl::Error, "Unable to give fd to socket!");
        exit(-2);
    }
}
