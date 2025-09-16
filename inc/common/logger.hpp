#pragma once

#include "common/notificaion.hpp"

#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>

#define COL_DEF "\033[0m"
#define COL_RED "\033[31m"
#define COL_YEL "\033[33m"
#define COL_GRY "\033[90m"

// to read logs you can use less -R 

// use std::source_location

// TODO
// send wayland notification on warrning/error?

enum class LogLvl : char {
	Info = 0,
	Warning = 1,
	Error = 2,
	None = 3
};

enum class LoggerOwner : bool {
    SecondThread,
    Other
};

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
    std::mutex _m;
	LogLvl _lvl;

	template <typename Head, typename... Tail>
	void subLog( Head head, Tail... tail );

	template <typename Head, typename... Tail>
	void subLog( Head last );

	void subLog();

public:
    // needed to unlock mutex if some signal handled while logging
    std::thread::id owner{};

	Logger( LogLvl = LogLvl::Warning );
	Logger( std::ofstream* of, LogLvl = LogLvl::Warning );
    Logger( const char* path, LogLvl = LogLvl::Warning );

    ~Logger();

	template <typename Head, typename... Tail>
	void log( LogLvl lvl, Head head, Tail... tail );

    // Need for signal hander function
    void unlock_mutex();
};

extern Logger logger;

template <typename Head, typename... Tail>
void Logger::log( LogLvl lvl, Head head, Tail... tail ) {
	if( lvl < _lvl ) return;

    owner = std::this_thread::get_id();
    std::lock_guard lg( _m );

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
