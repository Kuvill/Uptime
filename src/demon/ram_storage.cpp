#include "inc/ram_storage.hpp"

void Storage::insert( const ProcessInfo& info ) {
	_storage[ std::move(info.name) ] += info.uptime;
}

