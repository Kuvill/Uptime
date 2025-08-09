#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

// allow user to specify path (e.g. in constexpr)
class AppInfo {
    std::vector<fs::path> paths;

public:
    AppInfo() {
        
    }

};
