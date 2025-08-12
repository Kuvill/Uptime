#include "common/logger.hpp"
#include "common/change_dir.hpp"
#include "common/aliases.hpp"

#include <cstdlib>
#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

std::string pushFrontHome( const std::string& path ) {
    char* homePath( std::getenv("HOME") );
    std::string resultPath( homePath );

    resultPath.append( std::move( path ));

    std::cerr << "I return this shit: " << homePath << '\n';

    return resultPath;
}


Logger::Logger( LogLvl lvl ) : _lvl(lvl) {}
Logger::Logger( std::ofstream* of, LogLvl lvl ) : _out(of), _lvl(lvl) {}
Logger::Logger( const char* path, LogLvl lvl ) : _lvl(lvl) {
    CheckDirectory();
    _out = new std::ofstream( path );
}

Logger::~Logger() {
    if( dynamic_cast<std::ofstream*>( _out ))
        delete _out;
}



void Logger::subLog() {
	*_out << COL_DEF;
	*_out << '\n';
}
