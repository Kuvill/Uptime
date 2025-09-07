#include <common/change_dir.hpp>
#include <common/aliases.hpp>
#include <common/utils.hpp>

#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

static bool changed = false;

// No mutex guard! (not even in plan)
void CheckDirectory() {
    if( !changed ) {
        changed = true;

        char* homeDir( blockingGetEnv("HOME") );

        if( !homeDir ) {
            throw std::runtime_error("Unable to get Home env!");
        }

        fs::path newPath = ( homeDir );
        newPath += "/.local/share/uptimer/";

        if( !fs::exists(newPath) ) 
            fs::create_directories( newPath );

        // check rw access
        auto stat = fs::status( newPath );
        if( (stat.permissions() & fs::perms::owner_read) == fs::perms::none ||
                (stat.permissions() & fs::perms::owner_write) == fs::perms::none ) {

            throw std::runtime_error("Not enought permissions in directory");
        }

        // should catch error here guess and try to use other path
        fs::current_path( newPath );
    }
}

