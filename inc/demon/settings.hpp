#pragma once

#include "demon/plugin.hpp"
#include <chrono>

#include <toml++/toml.hpp>

class Settings final : Plugin {
    toml::table _config;
    const std::string_view _path;

public:
    Settings( std::string_view path = "~/.config/uptimer/conf.toml" );

    void reload();

    void OnTrigger() override;

    // --- variables
    // what if make then subscribers - notify all subscribers on change?
    // + - variables locality
    // - - overcomplexity. Not sure about ww for tmp variables (ll 100%. mb store and value also?)

    std::chrono::seconds cd;
};

