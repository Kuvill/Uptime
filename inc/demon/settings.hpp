#pragma once

#include "common/settings.hpp"
#include "demon/plugin.hpp"

// double deriving PepoHappy
class NotifySettings final : public Settings, public Plugin {
public:
    NotifySettings();
    NotifySettings( std::string_view path );

    void OnTrigger() override;
};
