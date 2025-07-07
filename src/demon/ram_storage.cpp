#include "common/logger.hpp"
#include "common/time.hpp"
#include "demon/ram_storage.hpp"

#include <ctime>
#include <functional>
#include <ostream>

#define USER_ID 1
const char NONAME[] = "";

Record::Record( uint32_t usr, Name appName, recTime_t uptime, recTime_t recTime, std::string describe )
            : user(usr), info(appName, uptime, describe), recTime( recTime ) {}

Record::Record( uint32_t usr, ProcessInfo info, recTime_t recTime )
            : user(usr), info(info), recTime(recTime) {}

// AI says, fnv-a1 would be nice
std::size_t std::hash<Record>::operator()( const Record& rec ) const {
	const std::hash<uint32_t> hasher;

	return (hasher(rec.user) << 32) + hasher(rec.recTime.count());
}

bool Record::operator==( const Record& other ) const {
	return recTime == other.recTime;
}

void Storage::insert( const ProcessInfo& info ) {
    auto time = getCurrentTime();
	
    insert( info, time );
}

void Storage::insert( const ProcessInfo& info, recTime_t time ) {
	if( info.name[0] == '\0' ) {
		logger.log(LogLvl::Warning, "The app has no app_id, skipping. (describe: ", info.describe, ")");
		return;
	}

	logger.log(LogLvl::Info, "new record: ", USER_ID, ", ", info.name.data(), ", ", info.uptime, ", ", time.count() );

	_storage.insert( Record(
		USER_ID,
        info,
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
        os << a.user << ' ' << a.info.name.data() << ' ' << a.info.uptime << ' '<< a.recTime.count() << '\n';
        os << a.info.describe << '\n';
	}

	return os;
}
