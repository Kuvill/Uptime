#pragma once

#include "inc/get_uptime.hpp"
#include "inc/time.hpp"

#include <unordered_set>

// cringe. i have to full rebuild this

struct Record {
	// FIXME should to save char* ( std::string coz i take from user, not lunux)
	uint32_t user;
    ProcessInfo info;
	recTime_t recTime;

	Record( uint32_t usr, Name appName, recTime_t uptime, recTime_t recTime, std::string describe = "" );
	Record( uint32_t usr, ProcessInfo info, recTime_t recTime );
    Record() = default;


	bool operator==( const Record& other ) const;
};

template<>
struct std::hash<Record> {
	size_t operator()( const Record& rec ) const;
};

struct Ranges {
	std::unordered_set<Record>::const_iterator begin, end;
};

class Storage {
	std::unordered_set<Record> _storage;
	using It = std::unordered_set<Record>::iterator;
	using cIt = std::unordered_set<Record>::const_iterator;

	friend std::ostream& operator<<( std::ostream& os, const Storage& store );

public:
	void insert( const ProcessInfo& info );
	void insert( const ProcessInfo& info, recTime_t time );

	It begin();

	It end();

	cIt begin() const;

	cIt end() const;

	// clear storage here
	void clear();
};
