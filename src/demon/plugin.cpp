#include <demon/ram_storage.hpp>
#include <demon/db.hpp>

extern Storage* g_store;
extern Database* g_db;

void saveProcessInfo( const ProcessInfo& info ) {
    if( !info.name.empty() )
        g_store->insert( info );
}


