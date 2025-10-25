#pragma once

#include "common/notificaion.hpp"

#include <iostream>
#include <fstream>


#define COL_DEF "\033[0m"
#define COL_RED "\033[31m"
#define COL_YEL "\033[33m"
#define COL_GRY "\033[90m"

// to read logs you can use less -R 

// use std::source_location

enum class LogLvl : char {
	Info = 0,
	Warning = 1,
	Error = 2,
	None = 3
};

#ifndef NOLOG

constexpr const char* toStr( LogLvl lvl ) {
	switch (lvl) {
		case LogLvl::Info: return COL_GRY "Info";
		case LogLvl::Warning: return COL_YEL "Warning";
		case LogLvl::Error: return COL_RED "Error";
		default: return "None";
	}
}

std::string pushFrontHome( std::string&& );

class Logger {
	std::ostream* _out = &std::clog;
	LogLvl _lvl;

	template <typename Head, typename... Tail>
	void subLog( Head head, Tail... tail );

	template <typename Head, typename... Tail>
	void subLog( Head last );

	void subLog();

public:
    void Init( LogLvl = LogLvl::Warning );
    void Init( std::ofstream* of, LogLvl = LogLvl::Warning );
    void Init( const char* path, LogLvl = LogLvl::Warning );

    ~Logger();

	template <typename Head, typename... Tail>
	void log( LogLvl lvl, Head head, Tail... tail );
};

extern Logger logger;

// ---- 
// template Implementations
// ---

template <typename Head, typename... Tail>
void Logger::log( LogLvl lvl, Head head, Tail... tail ) {
	if( lvl < _lvl ) return;

    if( lvl == LogLvl::Error )
         notificate( head ); 


	*_out << toStr(lvl) << ": ";
	*_out << head;
	subLog( tail... );
}

template <typename Head, typename... Tail>
void Logger::subLog( Head head, Tail... tail ) {
	*_out << head;
	subLog( tail... );
}

template <typename Head, typename... Tail>
void Logger::subLog( Head last ) {
	*_out << last << '\n';
    *_out << std::flush;
	*_out << COL_DEF;
}

#else

class /* __attribute__((visibility("hidden"))) */ Logger {
public:
   	Logger( LogLvl = LogLvl::Warning ) noexcept {}
	Logger( std::ofstream* of, LogLvl = LogLvl::Warning ) noexcept {}
    Logger( const char* path, LogLvl = LogLvl::Warning ) noexcept {}

    template<typename Head, typename... Tail>
    void log( LogLvl, Head&&, Tail... ) noexcept {}
};

inline Logger logger{LogLvl::Info};

#endif
