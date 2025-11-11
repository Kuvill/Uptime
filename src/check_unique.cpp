#include "common/check_unique.hpp"
#include "common/logger.hpp"
#include "common/settings.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <unistd.h>

namespace fs = std::filesystem;

static const std::string_view lockFile = "in_use.lock";
static const fs::path procFile = "/proc/";

CheckUnique::CheckUnique() {
    std::string path( settings_->value_or({ PATH_LABEL, "lock_file" }, SHARE_PATH) );
    path += lockFile;

    if( fs::exists( path ) ) {
        
        // How should i control old path?

        std::ifstream file( path );
        std::string pid;

        file >> pid;

        if( fs::exists( procFile.string() + pid ) ) {
            logger.log( LogLvl::Error, "Application not unique. Terminate" );
            throw std::runtime_error("Lock file found! Application already execute"
                "or an incedent with process accured!");
            
        } else {
            logger.log(LogLvl::Warning, "Lock file wrong. Previous session closed incorrect.");
        }
    } 

    // clear file
    int pid = getpid();
    std::ofstream file( path, std::ofstream::out | std::ofstream::trunc );

    file << pid;
    logger.log(LogLvl::Info, "Application instace is unique.");

}

CheckUnique::~CheckUnique() {
    delete_lock_file();
}

void delete_lock_file() {
    std::string path( settings_->value_or({ PATH_LABEL, "lock_file" }, SHARE_PATH) );
    path += lockFile;

    fs::remove( path );
}
