#pragma once

#include "demon/epoll.hpp"
#include <common/logger.hpp>

#include <stdexcept>
#include <cstring>

#include <sys/timerfd.h>
#include <unistd.h>

class TimerEvent : Plugin {
    int _fd;
    itimerspec _settings;

public:
    TimerEvent() {
        errno = 0;
        _fd = timerfd_create(CLOCK_MONOTONIC, 0);

        if( _fd < 0 ) {
            logger.log(LogLvl::Error, "Unable to create eventfd (", _fd, "): ", strerror(errno) );
            throw std::runtime_error("Unable to create eventfd");
        }

        _settings.it_interval.tv_sec = 5;
        _settings.it_interval.tv_nsec = 0;
        _settings.it_value.tv_nsec = 1;
        _settings.it_value.tv_sec = 0;

        // it value - initial sleep. 
        timerfd_settime( _fd, 0, &_settings, nullptr );

        Subscribe( _fd, this );
    }

    void OnTrigger() override {
        logger.log(LogLvl::Warning, "Motion on timer doesn't implemented yet!");
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
