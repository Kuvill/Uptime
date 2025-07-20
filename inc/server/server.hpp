#pragma once

#include "server/connection.hpp"

#include <netinet/in.h>
#include <unordered_map>

constexpr const char* PATH = "/tmp/UptimeServ.sock";

struct IPv6{};

class Server {
    int _serverSocket, _siteSocket;
    std::unordered_map<in_addr_t, Connection> _clients;

public:
    // protocol should be port
    Server( unsigned short protocol );
    Server( unsigned short protocol, IPv6 );
    ~Server() = default;

    void run();
};

enum class MessageType : char {
    SendDB,
    QuerryStat
};

struct Message {
    MessageType type;
    void* __;
};

