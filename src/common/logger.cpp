#ifndef NOLOG

#include "common/logger.hpp"
#include "common/aliases.hpp"
#include <common/settings.hpp>

#include <ostream>


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

void Logger::Init( std::string_view fileName, LogLvl lvl ) {
    Init( lvl );

    std::string path{ settings_->value_or( {PATH_LABEL, "logger"}, SHARE_PATH ) };
    path += fileName;

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

#endif // NOLOG
