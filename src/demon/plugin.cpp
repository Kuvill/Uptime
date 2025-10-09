#include "common/time.hpp"
#include <chrono>
#include <demon/ram_storage.hpp>
#include <demon/db.hpp>

extern Storage* g_store;
extern Database* g_db;

static ProcessInfo prevName;
static bool prevSkipped = false;

void saveProcessInfo( const ProcessInfo& info ) {
    if( info.name.empty() )
        return;

    if( getCurrentTime() - prevName.uptime <= std::chrono::seconds(1) );

    if( prevSkipped ) {
        
    }

    g_store->insert( info );
}


