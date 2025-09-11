#include <demon/signals.hpp>

#include <csignal>

// for current thread
void blockInteruptSignals() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGABRT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

}

// for current thread
void allowInteruptSignals() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGABRT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_UNBLOCK, &set, NULL);

}

void setCallbackForInteruptSignals( void (*callback)( int ) ) {
	signal( SIGINT, callback );
	signal( SIGABRT, callback );
	signal( SIGTERM, callback );
}
