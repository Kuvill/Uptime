#include "common/logger.hpp"

#include <cstdlib>
#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

// migrate to filesystem
ChangeDir::ChangeDir() {
    std::string newPath = std::move(std::getenv("HOME"));
    newPath += "/.local/share/uptimer/";

    struct stat st;
    if (stat(newPath.c_str(), &st) == -1) {
        mkdir(newPath.c_str(), 0777);
    }

    if (chdir( newPath.c_str() ) != 0)
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
    if( dynamic_cast<std::ofstream*>( _out ))
        delete _out;
}



void Logger::subLog() {
	*_out << COL_DEF;
	*_out << '\n';
}
