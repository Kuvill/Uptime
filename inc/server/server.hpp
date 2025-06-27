#pragma once

#include "server/connection.hpp"

#include <vector>

constexpr const char* PATH = "/tmp/UptimeServ.sock";

class Server {
    int _serverSocket;
    std::vector<Connection> _clients;

public:
    // protocol should be port
    Server( const char* protocol );
    ~Server() = default;

    void run();
};
