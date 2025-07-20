#pragma once

#include <cstdint>
#include <string>
#include "common/time.hpp"

using Name = std::array<char, 40>;

struct ProcessInfo {
	Name name{};
	recTime_t uptime;
    std::string describe;
};

ProcessInfo FocusInfo();
