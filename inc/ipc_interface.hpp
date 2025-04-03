#pragma once

#define SOCK_PATH "/tmp/uptime.sock"

// it is char to simplify cast to message
enum class MsgType : char {
	start_record,
	end_record,
	change_cd,

	none
};


// as it constexpr, std::string will allocate in compile-time
constexpr const char ReturnCode( MsgType code ) {
	return static_cast<char>( code );
}
