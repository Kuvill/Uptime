#include "common/logger.hpp"
#include "common/time.hpp"
#include "demon/ram_storage.hpp"

#include <ctime>
#include <functional>
#include <ostream>

#define USER_ID 1

Record::Record( uint32_t usr, ProcessInfo info )
            : user(usr), info(info) {}

// AI says, fnv-a1 would be nice
std::size_t std::hash<Record>::operator()( const Record& rec ) const {
	const std::hash<uint32_t> hasher;

	return (hasher(rec.user) << 32) + hasher(rec.info.timestomp.count());
}

bool Record::operator==( const Record& other ) const {
	return info.timestomp == other.info.timestomp;
}

void Storage::insert( const ProcessInfo& info ) {
	logger.log(LogLvl::Info, "new record: ", USER_ID, ", ", info.name.data(), ", ", info.timestomp );

	_storage.insert( Record(
		USER_ID,
        info
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

size_t Storage::size() {
    return _storage.size();
}

std::ostream& operator<<( std::ostream& os, const Storage& store ) {
	os << "Ram info:\n";
	for( auto& a : store ) {
        os << a.user << ' ' << a.info.name.data() << ' ' << a.info.timestomp << ' '<< a.info.timestomp.count() << '\n';
        os << a.info.describe << '\n';
	}

	return os;
}
