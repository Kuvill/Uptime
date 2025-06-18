#pragma once

#include "inc/ipc_interface.hpp"

#include <cerrno>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>

class Ips {
	int _serverSocket;
	char _addInfo[ MsgSize ];

public:
	Ips();
	~Ips();

	MsgType listen();
	const char* getMessage();
};
