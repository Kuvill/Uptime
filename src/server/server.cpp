#include "server/server.hpp"
#include "common/logger.hpp"

#include <cstdio>
#include <cstdlib>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

Server::Server( const char* protocol ) {
    /*
    _serverSocket = socket( AF_INET6, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if( _serverSocket < 0 ) {
        logger.log(LogLvl::Error, "Unable to create server socket!");
        exit(-1);
    }
    */

    /*
    addrinfo hint, *host;
    hint.ai_family = AF_INET6;
    hint.ai_socktype = SOCK_STREAM | SOCK_NONBLOCK;

    if( auto a = getaddrinfo( nullptr, protocol, &hint, &host ) != 0 ) {
        logger.log(LogLvl::Error, "Unable to get host info!", a);
        exit(-1);
    }

    _serverSocket = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
    if( _serverSocket < 0 ) {
        logger.log(LogLvl::Error, "Unable to crete socket!");
    }
    // inet_pton( )

    freeaddrinfo(host);
    */

    struct ifaddrs *ifaddr;
    char addr[INET6_ADDRSTRLEN];

    if( getifaddrs(&ifaddr) <0 -1 ) {
        logger.log(LogLvl::Error, "Unable to get host info!");
        exit(-1);
    }

    // Loop through the interfaces
    for( auto ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next ) {
        // Check if the interface is up and has an address
        if (ifa->ifa_addr == nullptr) continue;

        // Check for IPv6 addresses
        if (ifa->ifa_addr->sa_family == AF_INET6) {
            // Convert the address to a string
            if (inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr, addr, sizeof(addr)) != nullptr) {
                std::cout << "Interface: " << ifa->ifa_name << " - IPv6 Address: " << addr << std::endl;
            } else {
                perror("inet_ntop");
            }
        }
    }

    freeifaddrs(ifaddr);

    sockaddr_in6 a;
    a.sin6_family = AF_INET6;
    a.sin6_addr = inet_pton( addr );

    if( bind(_serverSocket, host->ai_addr, host->ai_addrlen) < 0 ) {
        logger.log(LogLvl::Error, "Unable to give fd to socket!");
        exit(-2);
    }


    if( listen( _serverSocket, SOMAXCONN ) ) {
        logger.log(LogLvl::Error, "Unable to open socket for connections!");
        exit(-3);
    }
}

void Server::run( ) {
    int clientSocket = -1;
    while( true ) {
        // mb needed accept4 with nonblock
        // clientSocket = accept( _serverSocket, )
        exit(0);
    }
}
