#pragma once

#include <common/logger.hpp>

#include <stdexcept>
#include <sys/timerfd.h>
#include <unistd.h>

class TimerEvent {
    int _fd;
    itimerspec _settings;

public:
    TimerEvent() {
        _fd = timerfd_create(CLOCK_REALTIME, 0);

        if( _fd <= 0 ) {
            throw std::runtime_error("Unable to create eventfd");
        }

        _settings.it_interval.tv_sec = 5;
        _settings.it_value.tv_nsec = 0;

        // it value - initial sleep. 
        timerfd_settime( _fd, 0, &_settings, nullptr );
    }

    void changeCD( int new_sec ) {
        logger.log(LogLvl::Info, "Timer event fresh rate changed! new value: ", new_sec);
        _settings.it_interval.tv_sec = new_sec; 
    }

    ~TimerEvent() {
        close( _fd );
    }

    operator int() {
        return _fd;
    }


};
