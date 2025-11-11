#include <common/change_dir.hpp>
#include <common/aliases.hpp>
#include <common/utils.hpp>

#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

static bool changed = false;

// No mutex guard! (not even in plan)
void ChangeDirectoryToHome() {
    if( !changed ) {
        changed = true;

        char* homeDir( blockingGetEnv("HOME") );

        fs::path newPath = ( std::move(homeDir) );

        auto stat = fs::status( newPath );
        if( (stat.permissions() & fs::perms::owner_read) == fs::perms::none ||
                (stat.permissions() & fs::perms::owner_write) == fs::perms::none ) {

            throw std::runtime_error("Not enought permissions in directory");
        }

        fs::current_path( newPath );
    }
}

