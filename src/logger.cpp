#include "common/logger.hpp"
#include "common/change_dir.hpp"
#include "common/aliases.hpp"
#include <cstring>
#include <ostream>

#ifndef NOLOG

#include <iostream>

#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

void Logger::Init( LogLvl lvl ) {
    _lvl = lvl;

    // since i use only logger to interact with user
    std::ios_base::sync_with_stdio( false );
    std::clog.tie( nullptr );
}


void Logger::Init( std::ofstream* of, LogLvl lvl ) {
    Init( lvl );
    _out = of;

    logger.log(LogLvl::Info, "Logger source has been changed.");
}

void Logger::Init( std::string_view directory, LogLvl lvl ) {
    Init( lvl );

    std::string path{ directory };
    path += "demon.log";

    _out = new std::ofstream( std::move(path) );

    logger.log(LogLvl::Info, "Logger source has been changed.");
}

void Logger::subLog() {
	*_out << COL_DEF;
    *_out << std::flush;
	*_out << '\n';
}

Logger::~Logger() {
    *_out << std::flush;
}

#endif
