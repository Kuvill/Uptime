#pragma once

#include <filesystem>
#include <string_view>
#include <vector>

// allow user to specify path (e.g. in constexpr)
// Most time XDG dirs are broken, so now i will not check it

// Btw bad naming. conflict with ProcessInfo
class AppInfo {
    std::vector<std::filesystem::path> paths;

    std::filesystem::path getDesktopEntryPath( std::string_view appName );

public:
    AppInfo();

    // return opened file instance? 
    std::string getComment( std::string_view appName );
    std::filesystem::path getIconPath( std::string_view appName );
};
