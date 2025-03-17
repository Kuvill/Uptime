#pragma once

#include "inc/get_uptime.hpp"

#include <unordered_map>

struct Ranges {
	std::unordered_map<Name, uint64_t>::const_iterator begin, end;
};

class Storage {
	std::unordered_map<Name, uint64_t> _storage;

public:
	void insert( const ProcessInfo& info );

	// clear storage here
	Ranges dump();
};
