#pragma once

#include <cstdint>
#include <string>
#include "inc/time.hpp"

// using Name = std::array<char, 40>;
using Name = std::string;
struct ProcessInfo {
	Name name{};
	recTime_t uptime;
    std::string describe;
};

ProcessInfo FocusInfo();
