#include "gui/client.hpp"
#include "common/ipc_interface.hpp"
#include "common/logger.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <cassert>

// Have to be non-blocking btw, since it Gui app
Client::Client() {
    logger.log(LogLvl::Info, "trying to create client socket...");

    _clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if( _clientSocket < 0 )
        throw std::runtime_error("cannot create client socket");

    sockaddr_un localAdr{};
    localAdr.sun_family = AF_UNIX;
    strcat( localAdr.sun_path, SOCK_PATH );

    if( connect(_clientSocket, reinterpret_cast<sockaddr*>( &localAdr ), sizeof(localAdr)) < 0 ) {
        // throw std::runtime_error("cannot connect to socket");
        is_there_server = false;
        return;
    }

    char msg = ReturnCode( MsgType::start_record );
    // if( send(_clientSocket, &msg, 1, 0) ) // since it non block it always thorw?
        // throw std::runtime_error("error while sending message");

    logger.log(LogLvl::Info, "socket craeted and message sended!");
}

Client::~Client() {
    logger.log(LogLvl::Info, "Sending end record message");
    char msg = ReturnCode( MsgType::end_record );
    if( send(_clientSocket, &msg, 1, 0) )
        logger.log(LogLvl::Error, "Message about closing client didn't resive!!!");

    close( _clientSocket );
}

void Client::changeCd( std::chrono::seconds newCd ) {
    logger.log(LogLvl::Info, "Sending message to change cooldown...");
    assert( sizeof(newCd) < MsgSize );
    char buf[ sizeof(newCd) ]{};

    std::memcpy( buf, std::move(reinterpret_cast<char*>( &newCd )), sizeof(newCd) );
    assert( reinterpret_cast<std::chrono::seconds&>( buf ) == newCd );

    if( !is_there_server )
        return;

    if( send( _clientSocket, buf, sizeof(newCd), 0) < 0 )
        throw std::runtime_error("error while sending new cd");
}
