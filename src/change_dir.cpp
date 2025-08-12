#include <common/change_dir.hpp>
#include <common/logger.hpp>

#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

static bool changed = false;

// No mutex guard! (not even in plan)
void CheckDirectory() {
    if( !changed ) {
        changed = true;

        const char* homeDir = std::getenv("HOME");

        if( !homeDir ) {
            logger.log(LogLvl::Error, "Unable to get Home env!");
            std::terminate();
        }
        fs::path newPath = ( homeDir );
        newPath += "/.local/share/uptimer/";

        if( !fs::exists(newPath) ) 
            fs::create_directories( newPath );

        // check rw access
        auto stat = fs::status( newPath );
        if( (stat.permissions() & fs::perms::owner_read) == fs::perms::none ||
                (stat.permissions() & fs::perms::owner_write) == fs::perms::none ) {

            logger.log(LogLvl::Error, "Not enought permissions (", newPath, ")");
            throw std::runtime_error("Not enought permissions in directory");
        }

        // should catch error here guess and try to use other path
        fs::current_path( newPath );
        logger.log(LogLvl::Info, "Directory changed (", newPath, ")");

    }
}

