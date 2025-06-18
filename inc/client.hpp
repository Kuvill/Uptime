#pragma once

#include <chrono>

class Client {
    int _clientSocket;
    bool is_there_server = true;
public:
    // send start record
    Client();

    // send end record
    ~Client();

    void changeCd( std::chrono::seconds );
};
