#pragma once

#include <print>

#ifndef DISABLE_NOTIFICATIONS
void notificate( const char* what ); 

template< typename T >
static inline void notificate( T ) {std::println("Wtf is going on");}

#else
template< typename T >
static inline void notificate( T ) {}
#endif

