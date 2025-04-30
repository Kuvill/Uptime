#include "inc/logger.hpp"

#include <unistd.h>
#include <cstdlib>
#include <fstream>

ChangeDir::ChangeDir(const char* path ) {
    if (chdir(path) != 0)
        exit(127);
}

std::string pushFrontHome( const std::string& path ) {
    std::string homePath = std::getenv("HOME");

    homePath.append( std::move( path ));

    std::cerr << "I return this shit: " << homePath << '\n';
    
    return homePath;
}


Logger::Logger( LogLvl lvl ) : _lvl(lvl) {}
Logger::Logger( std::ofstream* of, LogLvl lvl ) : _out(of), _lvl(lvl) {}
Logger::Logger( const char* path, LogLvl lvl ) : _lvl(lvl) {
    _out = new std::ofstream( path );
}

Logger::~Logger() {
    delete _out;
}



void Logger::subLog() {
	*_out << COL_DEF;
	*_out << '\n';
}
