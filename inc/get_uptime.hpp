#pragma once

#include <cstdint>
#include <string>

// using Name = std::array<char, 40>;
using Name = std::string;
struct ProcessInfo {
	Name name{};
	std::uint32_t uptime;
};

ProcessInfo FocusInfo();
