#include "common/check_unique.hpp"
#include "common/logger.hpp"
#include "common/change_dir.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <unistd.h>

namespace fs = std::filesystem;

static const fs::path lockFile = "in_use.lock";
static const fs::path procFile = "/proc/";

CheckUnique::CheckUnique() {
    CheckDirectory();

    if( fs::exists( lockFile ) ) {
        
        std::ifstream file( lockFile );
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
    std::ofstream file( lockFile, std::ofstream::out | std::ofstream::trunc );

    file << pid;
    logger.log(LogLvl::Info, "Application instace is unique.");

}

CheckUnique::~CheckUnique() {
    delete_lock_file();
}

void delete_lock_file() {
    fs::remove( lockFile );
}
