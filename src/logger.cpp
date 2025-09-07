#include "common/logger.hpp"
#include "common/change_dir.hpp"
#include "common/aliases.hpp"

#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

Logger::Logger( LogLvl lvl ) : _lvl(lvl) {}
Logger::Logger( std::ofstream* of, LogLvl lvl ) : _lvl(lvl) {
    logger.log(LogLvl::Info, "Logger source has been changed.");
    _out = of;
}
Logger::Logger( const char* path, LogLvl lvl ) : _lvl(lvl) {
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
