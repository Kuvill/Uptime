#pragma once

#include <cstdint>
#include <array>
#include <string>

// using Name = std::array<char, 40>;
using Name = std::string;
struct ProcessInfo {
	Name name{};
	std::size_t uptime;
};

ProcessInfo FocusInfo();
