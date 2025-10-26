#pragma once 

#include "common/logger.hpp"
#include "demon/get_uptime.hpp"

struct Plugin {
private: int _fd;
public:

    int setFd( int fd ){ logger.log(LogLvl::Info, "set fd: ", fd ); _fd = fd; return _fd; }
    int getFd(){ logger.log(LogLvl::Info, "get fd"); return _fd; }

    virtual void OnTrigger() = 0;
    virtual ~Plugin() = default;
};

void saveProcessInfo( const ProcessInfo& info );
