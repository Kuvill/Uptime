#pragma once

#include <csignal>

#include <sys/signalfd.h>

class SignalEvent {
    int fd;

public:
    SignalEvent() {
        sigset_t ss;

        sigemptyset( &ss );
        sigaddset(&ss, SIGINT);
        sigaddset(&ss, SIGTERM);
        sigaddset(&ss, SIGQUIT);
        sigaddset(&ss, SIGSEGV);

        fd = signalfd( -1, &ss, SFD_NONBLOCK );
    }

    operator int() {
        return fd;
    }
};
