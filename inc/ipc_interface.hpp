#pragma once

#define SOCK_PATH "/tmp/uptime.sock"

enum class MsgType {
	start,
	end,
	none,
	debug
};


