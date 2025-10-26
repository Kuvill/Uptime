#pragma once

#include "demon/plugin.hpp"

#include <chrono>

#include <filesystem>
#include <toml++/toml.hpp>

struct Paths {
    std::filesystem::path share;
    std::filesystem::path state;
    std::filesystem::path config;
    std::filesystem::path cache;
};

class Settings final : public Plugin {
    toml::table _config;

    void setupPaths();
    void setupThePath( std::string_view variable, std::string_view def );

public:
    Settings( std::string_view overridenConfPath );
    ~Settings() = default;

    void reload();

    void OnTrigger() override;

    // --- variables
    // what if make then subscribers - notify all subscribers on change?
    // + - variables locality
    // - - overcomplexity. Not sure about ww for tmp variables (ll 100%. mb store and value also?)

    std::chrono::seconds cd;
    Paths paths;
};

