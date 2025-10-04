#pragma once 

#include "demon/get_uptime.hpp"

struct Plugin {
    int _fd;

    virtual void OnTrigger() = 0;
};

void saveProcessInfo( const ProcessInfo& info );
