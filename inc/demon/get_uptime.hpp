#pragma once

#include <ostream>
#include <string>
#include "common/time.hpp"

using Name = std::array<char, 40>;

struct ProcessInfo {
	Name name{};
	recTime_t uptime;
    std::string describe;

    bool operator!=( const ProcessInfo& other ) const {
        return name != other.name ||
            uptime != other.uptime ||
            describe != other.describe;
    }
};

std::ostream& operator<<( std::ostream& os, const ProcessInfo& info );

ProcessInfo FocusInfo();
