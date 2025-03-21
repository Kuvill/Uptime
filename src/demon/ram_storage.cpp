#include "inc/ram_storage.hpp"

#include <ctime>
#include <functional>

#define USER_ID 1

// AI says, fnv-a1 would be nice
std::size_t std::hash<Record>::operator()( const Record& rec ) const {
	const std::hash<uint32_t> hasher;

	return (hasher(rec.user) << 32) + hasher(rec.recTime);
}

bool Record::operator==( const Record& other ) const {
	return recTime == other.recTime;
}

void Storage::insert( const ProcessInfo& info ) {
	std::time_t time;
	std::time(&time);

	_storage.insert( Record(
		USER_ID,
		info.name,
		info.uptime,
		time
	 ));
}

Storage::It Storage::begin() {
	return _storage.begin();
}

Storage::It Storage::end() {
	return _storage.end();
}

Storage::cIt Storage::cbegin() const {
	return _storage.cbegin();
}

Storage::cIt Storage::end() const {
	return _storage.end();
}

void Storage::clear() {
	_storage.clear();
}

