#pragma once

#include <filesystem>

namespace Xdg {
    extern std::filesystem::path config;
    extern std::filesystem::path share;
    extern std::filesystem::path state;
    extern std::filesystem::path cache;
};
