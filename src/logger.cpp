#include "common/logger.hpp"
#include "common/change_dir.hpp"
#include "common/aliases.hpp"

#ifndef NOLOG

// thx for +300ms to compilation just for untie
#include <iostream>

#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

Logger::Logger( LogLvl lvl ) : _lvl(lvl) {
    // since i use only logger to interact with user
    std::ios_base::sync_with_stdio( false );
    std::clog.tie( nullptr );
}


Logger::Logger( std::ofstream* of, LogLvl lvl ) : Logger(lvl) {
    logger.log(LogLvl::Info, "Logger source has been changed.");
    _out = of;
}
Logger::Logger( const char* path, LogLvl lvl ) : Logger(lvl) {
    CheckDirectory();
    logger.log(LogLvl::Info, "Logger source has been changed.");
    _out = new std::ofstream( path );
}

Logger::~Logger() {
    if( dynamic_cast<std::ofstream*>( _out ))
        delete _out;
}

void Logger::subLog() {
	*_out << COL_DEF;
    *_out << std::flush;
	*_out << '\n';
}

#endif
