// to do: recive nubmer of interval

#include "common/ipc_interface.hpp"
#include "common/logger.hpp"
#include "common/settings.hpp"
#include "demon/server.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <stdexcept>

#include <cerrno>
#include <cstring>

static MsgType TypeByMsg( char buf ) {
	return static_cast<MsgType>( buf );
}

Ips::Ips() {
	_serverSocket = socket( AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0 );
	if( _serverSocket  < 0 )
		throw std::runtime_error("Cannot create an socket!");

	{
		sockaddr_un localAdr{};
		localAdr.sun_family = AF_UNIX;

        std::string path( settings_->value_or({ PATH_LABEL, "socket" }, SHARE_PATH ) );
        path += SOCK_PATH;

		strcpy( localAdr.sun_path, path.c_str() );

		// if socket exist
		unlink( path.c_str() );
		if( bind( _serverSocket, reinterpret_cast<sockaddr*>(&localAdr), 
            sizeof(localAdr) ) < 0 ) {
                    logger.log(LogLvl::Error, strerror(errno));
                    throw std::runtime_error("failed to bind socket!");
        }
	}

	if( ::listen( _serverSocket, 10 ) < 0 )
		throw std::runtime_error("Failed to connect to socket!");
}

Ips::~Ips() {
    std::string path( settings_->value_or({ PATH_LABEL, "socket" }, SHARE_PATH ) );
    path += SOCK_PATH;

	close( _serverSocket );
	unlink( path.c_str() );
}

MsgType Ips::listen() {
	char buf[16]{};

	int clientSocket = -1;

		if( clientSocket < 0 ) 
			clientSocket = accept( _serverSocket, nullptr, nullptr );
		if( clientSocket >= 0 ) {
			logger.log( LogLvl::Info, "Client Found");
			int rCode = read( clientSocket, &buf, 15 );

			if( rCode < 0 )
				clientSocket = -1;

			if( rCode > 0 ) {
				strcpy( _addInfo, buf+1 );
				return TypeByMsg( buf[0] );
			}
			
		} else {
			if( errno != EWOULDBLOCK ) 
				throw std::runtime_error("scary, really scary. Would we known what it is...");
		}
	
	return MsgType::none;
}

const char* Ips::getMessage() {
	return _addInfo;
}
