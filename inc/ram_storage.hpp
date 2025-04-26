#pragma once

#include "inc/get_uptime.hpp"
#include "inc/time.hpp"

#include <unordered_set>

// cringe. i have to full rebuild this

struct Record {
	// FIXME should to save char* ( std::string coz i take from user, not lunux)
	uint32_t user;
	uint32_t uptime;
	Name name;
	recTime_t recTime;

	Record( uint32_t u, Name n, uint32_t p, recTime_t recTime ) : user(u), uptime(p), name(n), recTime(recTime) {}
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
