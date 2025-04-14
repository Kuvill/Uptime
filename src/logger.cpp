#include "inc/logger.hpp"

#ifndef Release 

void DEBUG( const std::string& text ) {
	std::cerr << text << '\n';
}

#else

void DEBUG( const std::string& text ) {}

#endif

Logger::Logger( LogLvl lvl ) : _lvl(lvl) {}
Logger::Logger( std::ofstream& of, LogLvl lvl ) : _out(of), _lvl(lvl) {}
Logger::~Logger() = default;

void Logger::subLog() {
	_out << COL_DEF;
	_out << '\n';
}
