#include <iostream>
#include <string>

#ifndef Release 

void DEBUG( const std::string& text ) {
	std::cerr << text << '\n';
}

#else

void DEBUG( const std::string& text ) {}

#endif

