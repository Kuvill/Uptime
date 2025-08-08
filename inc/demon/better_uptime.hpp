#pragma once

#include "demon/get_uptime.hpp"

recTime_t ps( std::array<char, 6> pid );
recTime_t ps( const unsigned char* );
recTime_t ps( const std::string& );

class DesktopEnv {
protected:
    // +: interface
    // -: small overhead
    virtual void castToBase();

public:
    DesktopEnv() = default;
    virtual ~DesktopEnv() = default;

    virtual ProcessInfo getFocused();

    virtual DesktopEnv* checkDE();
};

class _SwayDE final : public DesktopEnv {
    int _sock;

    void castToBase() override;

public:
    _SwayDE();
    ~_SwayDE();

    ProcessInfo getFocused() override;

    DesktopEnv* checkDE() override;
};

class _Hyprland final : public DesktopEnv {
    int _sock;

    void castToBase() override;

public:
    _Hyprland();
    ~_Hyprland();

    ProcessInfo getFocused() override;

    DesktopEnv* checkDE() override;
};
