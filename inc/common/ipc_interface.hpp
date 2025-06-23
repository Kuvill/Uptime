#pragma once

// should it be static?
static const char SOCK_PATH[] = "/tmp/uptime.sock";

static const int MsgSize = 8;

// it is char to simplify cast to message
enum class MsgType : char {
	start_record,
	end_record,
	change_cd,

	none
};


// as it constexpr, std::string will allocate in compile-time
// wtf i mean here ?)
constexpr const char ReturnCode( MsgType code ) {
	return static_cast<char>( code );
}
