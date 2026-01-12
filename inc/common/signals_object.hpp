#include "common/logger.hpp"
#include <cstring>

#include <csignal>
#include <stdexcept>
#include <sys/signalfd.h>
#include <unistd.h>
class Signals {
    int _fd;
    
public:
    // Interrupt EVERY possible interrupt to save data
    // may be except SIGSEGV?
    // except Trap and alarm
    Signals() {
        sigset_t signals;
        sigemptyset( &signals );
        sigaddset( &signals, SIGINT );
        sigaddset( &signals, SIGKILL );
        sigaddset( &signals, SIGABRT );
        sigaddset( &signals, SIGFPE );
        sigaddset( &signals, SIGSEGV );
        sigaddset( &signals, SIGTERM );

        sigaddset( &signals, SIGHUP	);
        sigaddset( &signals, SIGQUIT );
        sigaddset( &signals, SIGKILL );
        sigaddset( &signals, SIGPIPE );

        _fd = signalfd(-1, &signals, SFD_CLOEXEC);

        if( _fd < 0 ) {
            logger.log(LogLvl::Error, "Unable to create signalfd (", _fd, "): ", strerror(errno) );
            throw std::runtime_error("Unable to create signalfd");
        }

        // block traditional signal catching
        if( sigprocmask(SIG_BLOCK, &signals, nullptr ) == -1 ) {
            logger.log(LogLvl::Error, "Unable to block signals!, ", strerror(errno));
            throw std::runtime_error("Unable to block signals!");
        }
    }

    ~Signals() {
        // Why commented? to make pgrogram down faster
        // unblock signal catching
        // sigset_t signals;
        // sigemptyset( &signals );
        // sigaddset( &signals, SIGINT );
        // sigaddset( &signals, SIGKILL );
        // sigaddset( &signals, SIGABRT );
        // sigaddset( &signals, SIGFPE );
        // sigaddset( &signals, SIGSEGV );
        // sigaddset( &signals, SIGTERM );
// 
        // sigaddset( &signals, SIGHUP	);
        // sigaddset( &signals, SIGQUIT );
        // sigaddset( &signals, SIGKILL );
        // sigaddset( &signals, SIGPIPE );
        // sigprocmask(SIG_BLOCK, &signals, nullptr );

        close( _fd );
    }

    void OnTrigger() {
        signalfd_siginfo sig;
        read( _fd, &sig, sizeof(sig) );

        throw std::runtime_error("Signal rised!"); // i am soo lazy today FIXME
    }

    operator int() {
        return _fd;
    }
};
