#pragma once 

#include "common/logger.hpp"
#include "demon/get_uptime.hpp"

struct Plugin {
private: 
    int _fd;
    const bool _autoclearSocket;
public:
    Plugin( bool autoclearSocket ) : _autoclearSocket( autoclearSocket ) {}
    virtual ~Plugin() = default;

    int setFd( int fd ){ logger.log(LogLvl::Info, "set fd: ", fd ); _fd = fd; return _fd; }
    int getFd(){ logger.log(LogLvl::Info, "get fd"); return _fd; }
    bool autoclean() const { return _autoclearSocket; }

    virtual void OnTrigger() = 0;
};

void saveProcessInfo( const ProcessInfo& info );
