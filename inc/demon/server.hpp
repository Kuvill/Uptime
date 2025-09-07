#pragma once

#include <atomic>

#include "common/ipc_interface.hpp"

enum class LockStatus {
    Terminate,
    SessionLock,
    NoLock
};

struct LockNotifier {
    std::atomic<LockStatus> _stat;


    void sensitiveSleep( long time );
    /*
    operator decltype( _stat )::value_type() {
        return _stat;
    }
    */
};

class Ips {
	int _serverSocket;
	char _addInfo[ MsgSize ];

public:
	Ips();
	~Ips();

	MsgType listen();
	const char* getMessage();

    operator int() {
        return _serverSocket;
    }
};
