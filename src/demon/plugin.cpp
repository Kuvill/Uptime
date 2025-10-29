#include <chrono>
#include <demon/ram_storage.hpp>
#include <demon/db.hpp>

extern Storage* g_store;
extern Database* g_db;

static ProcessInfo prevInfo;
static bool prevSkipped = false;

void saveProcessInfo( const ProcessInfo& info ) {
    if( info.name.empty() ) {
        logger.log(LogLvl::Info, "Getted empty info");
        return;
    }

    if( info.timestomp - prevInfo.timestomp <= std::chrono::seconds(1) ) {
        logger.log(LogLvl::Info, "Focus changing swap detecting: do not insert last window");
        prevInfo = info;
        prevSkipped = true;

        return;
    }

    if( prevSkipped ) [[unlikely]] {
        logger.log(LogLvl::Info, "Previous Info inserted");
        g_store->insert( prevInfo );
        prevSkipped = false;
    }

    g_store->insert( info );
    prevInfo = info;
}


