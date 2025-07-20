#include "common/logger.hpp"
#include "server/server.hpp"
#include "server/coroutine.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>

#include <errno.h>
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

// btw ipv6 more safity for home server. in ddos case i can just turn off server
// and router will be good
const char* LOCAL_IP = "192.168.31.79";
const int CONNECTIONS_LIMIT = 1000;

// btw i don't need coroutines here, but i want it just for fun

Server::Server( unsigned short protocol ) {
    _serverSocket = socket( AF_INET, SOL_SOCKET | SOCK_NONBLOCK, 0 );
    if( _serverSocket < 0 ) {
        logger.log(LogLvl::Error, "Unable to create server socket!");
        exit(-1);
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons( protocol );
    addr.sin_addr.s_addr = inet_addr( LOCAL_IP );

    if( bind( _serverSocket, (sockaddr*)&addr, sizeof(addr) == 0 ) ) {
        logger.log(LogLvl::Error, "Unable to give and fd for socket!");
        exit(-2);
    }

    listen(_serverSocket, CONNECTIONS_LIMIT);
}

/*
Server::Server( const char* protocol, IPv6 ) {
    *
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
    /

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
*/

// sometimes i should save stat into db in case server reboot
void Server::run( ) {
    int clientSocket = -1;

    while( true ) {
        // mb needed accept4 with nonblock
        clientSocket = accept( _serverSocket, nullptr, nullptr );
        _clients[clientSocket].registrate();

        char buf[64];
        int rc;
        
        // Process desktop app message
        {
            Message msg;
            rc = read(clientSocket, buf, 1);
            if( rc > 0 )
                switch( msg.type ) {
                    case MessageType::SendDB:
                        break;

                    case MessageType::QuerryStat:
                        logger.log(LogLvl::Error, "Querring stat in desktop application not in plan.");
                        break;
                }
            else if( !( rc == -1 && (errno == EAGAIN || errno == EWOULDBLOCK) ) )
                logger.log(LogLvl::Error, "Error with reading ");
            
        }

        // Process site message
        read( _siteSocket, buf, 1 );
        
        

        // run corountime to be able listen next connection. 
        // Should be hiden into 

        // cowait
    }
}
