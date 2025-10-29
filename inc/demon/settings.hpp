#pragma once

#include "demon/plugin.hpp"

#include <chrono>

#include <filesystem>
#include <toml++/toml.hpp>

struct Paths {
    std::filesystem::path config;

    std::filesystem::path db;
    std::filesystem::path socket;
    std::filesystem::path log;
    std::filesystem::path lock;
};

class Settings final : public Plugin {
    toml::table _config;

    void setupPaths();
    std::string setupThePath( std::string_view variable, std::string_view def );

public:
    Settings( std::string_view overridenConfPath );
    ~Settings() = default;

    void reload();

    void OnTrigger() override;

    // --- variables
    // what if make then subscribers - notify all subscribers on change?
    // + - variables locality
    // - - overcomplexity. Not sure about ww for tmp variables (ll 100%. mb store and value also?)

    // just a links - bad idea for many cases (e.g. paths)
    // should use callbacks

    // also place conditions in callbacks for easy mantain?

    std::chrono::seconds cd;
    Paths paths;
};

extern Settings *settings_;
