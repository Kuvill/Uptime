#include "inc/logger.hpp"
#include "inc/ram_storage.hpp"

#include <ctime>
#include <functional>
#include <ostream>

#define USER_ID 1

// AI says, fnv-a1 would be nice
std::size_t std::hash<Record>::operator()( const Record& rec ) const {
	const std::hash<uint32_t> hasher;

	return (hasher(rec.user) << 32) + hasher(rec.recTime.time_since_epoch().count());
}

bool Record::operator==( const Record& other ) const {
	return recTime == other.recTime;
}

void Storage::insert( const ProcessInfo& info ) {
    auto time = std::chrono::system_clock::now();
	
    insert( info, time );
}

void Storage::insert( const ProcessInfo& info, recTime_t time ) {
	if( info.name[0] == '\0' ) {
		logger.log(LogLvl::Warning, "The app has no app_id, skipping");
		return;
	}

	logger.log(LogLvl::Info, "new record: ", USER_ID, ", ", info.name, ", ", info.uptime, ", ", time.time_since_epoch().count() );

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

Storage::cIt Storage::begin() const {
	return _storage.begin();
}

Storage::cIt Storage::end() const {
	return _storage.end();
}

void Storage::clear() {
	_storage.clear();
}

std::ostream& operator<<( std::ostream& os, const Storage& store ) {
	os << "Ram info:\n";
	for( auto& a : store ) {
		os << a.user << ' ' << a.name << ' ' << a.uptime << ' '<< a.recTime.time_since_epoch().count() << '\n';
	}

	return os;
}
