// to do: recive nubmer of interval

#include <inc/ipc_interface.hpp>
#include <inc/server.hpp>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <stdexcept>

// this is really potential problem FIXME
#include <cerrno>
#include "inc/logger.hpp"

const char start[] = "s";
const char end[] = "e";

MsgType TypeByMsg( const char buf[] ) {
	if( strcmp(buf, start) )
		return MsgType::start;
	else if( strcmp( buf, end ) )
		return MsgType::end;
	else
		return MsgType::debug;
}

Ips::Ips() {
	_serverSocket = socket( AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0 );
	if( _serverSocket  < 0 )
		throw std::runtime_error("Cannot create an socket!");

	{
		sockaddr_un localAdr{};
		localAdr.sun_family = AF_UNIX;
		strcpy( localAdr.sun_path, SOCK_PATH );

		// if socket exist
		unlink(SOCK_PATH);
		if( bind( _serverSocket, reinterpret_cast<sockaddr*>(&localAdr), 
					sizeof(localAdr) ) < 0 )
			throw std::runtime_error("failed to bind socket!");
	}

	if( ::listen( _serverSocket, 10 ) < 0 )
		throw std::runtime_error("Failed to connect to socket!");
}

Ips::~Ips() {
	close( _serverSocket );
	unlink(SOCK_PATH);
}

MsgType Ips::listen() {
	char buf[16]{};

	int clientSocket = -1;

		if( clientSocket < 0 ) 
			clientSocket = accept( _serverSocket, nullptr, nullptr );
		if( clientSocket >= 0 ) {
			logger.log( LogLvl::Info, "Client Found");
			int rCode = read( clientSocket, buf, 15 );

			if( rCode < 0 )
				clientSocket = -1;

			if( rCode > 0 ) {
				return TypeByMsg( buf );
			}
			
		} else {
			if( errno != EWOULDBLOCK ) 
				throw std::runtime_error("scary, really scary. Would we known what it is...");
		}
	
	return MsgType::none;
}
