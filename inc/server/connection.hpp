#pragma once

#include <cstdint>

class Connection {
    uint32_t connectCount;

    bool _connected;

public:
    void registrate();

};
