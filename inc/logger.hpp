#pragma once

#include <iostream>
#include <fstream>

#define LOG_PATH ".local/share/uptimer/";

#define COL_DEF "\033[0m"
#define COL_RED "\033[31m"
#define COL_YEL "\033[33m"
#define COL_GRY "\033[90m"

// add GUI / DEMON befor message

enum class LogLvl {
	Info = 0,
	Warning = 1,
	Error = 2,
	None = 3
};

constexpr const char* toStr( LogLvl lvl ) {
	switch (lvl) {
		case LogLvl::Info: return COL_GRY "Info";
		case LogLvl::Warning: return COL_YEL "Warning";
		case LogLvl::Error: return COL_RED "Error";
		default: return "None";
	}
}

class ChangeDir {
public:
    ChangeDir();
};

std::string pushFrontHome( std::string&& );

class Logger {
	std::ostream* _out = &std::cout;
	LogLvl _lvl;

	template <typename Head, typename... Tail>
	void subLog( Head head, Tail... tail );

	template <typename Head, typename... Tail>
	void subLog( Head last );

	void subLog();

public:
	Logger( LogLvl = LogLvl::Warning );
	Logger( std::ofstream* of, LogLvl = LogLvl::Warning );
    Logger( const char* path, LogLvl = LogLvl::Warning );

    ~Logger();

	template <typename Head, typename... Tail>
	void log( LogLvl lvl, Head head, Tail... tail );
};

extern Logger logger;

template <typename Head, typename... Tail>
void Logger::log( LogLvl lvl, Head head, Tail... tail ) {
	if( lvl < _lvl ) return;

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
	*_out << COL_DEF;
}
