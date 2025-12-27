#pragma once

#include "common/settings.hpp"
#include "demon/plugin.hpp"

class NotifySettings final : public Settings {
    int _fd;
        
public:
    NotifySettings();
    NotifySettings( std::string_view path );

    ~NotifySettings();

    void OnTrigger();
};
