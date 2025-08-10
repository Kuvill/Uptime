#include "gui/app_info.hpp"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string_view>

namespace fs = std::filesystem;

// home smaller so i will search here first
const static char* APP_DIRS[] {
     "~/.local/share/applications/",
     "/usr/share/applications/",
};

// return empty string if no entries
fs::path AppInfo::getDesktopEntryPath( std::string_view appName ) {

    for( auto path : paths ) {
        std::string searchedPath = path; searchedPath.append( appName );

        for( const auto& entry : fs::recursive_directory_iterator( path) ) {
            if( entry.is_regular_file() ) {

                if( entry.path() == searchedPath ) {
                    return entry.path();
                }
            }
        }
    }

    return std::string();
}

AppInfo::AppInfo() {
    for( auto path : APP_DIRS ) {
        if( fs::exists( path ))   
            paths.emplace_back( path );
    }
}

std::string AppInfo::getComment( std::string_view appName ) {
    std::ifstream file( getDesktopEntryPath( appName ) );

    std::istream_iterator<char> begin( file ), end;
    std::vector<char> f = { begin, end };
}
