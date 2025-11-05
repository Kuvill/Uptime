#include <common/xdg_dirs.hpp>

// parsed from settings, not args

// but runtime change - scarry
// So it ok create as stack member, then delete it
namespace Xdg {
    std::filesystem::path config;
    std::filesystem::path share;
    std::filesystem::path state;
    std::filesystem::path cache;
};
