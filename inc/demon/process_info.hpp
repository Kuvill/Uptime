#pragma once

#include "common/time.hpp"

#include <ostream>
#include <string>

// using Name = std::array<char, 40>;
using Name = std::string;

struct ProcessInfo {
	Name name{};
	recTime_t timestomp;
    std::string describe;

    bool operator!=( const ProcessInfo& other ) const {
        return name != other.name ||
            timestomp != other.timestomp ||
            describe != other.describe;
    }
};

std::ostream& operator<<( std::ostream& os, const ProcessInfo& info );
