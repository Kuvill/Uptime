#pragma once

#include "demon/get_uptime.hpp"
#include <common/aliases.hpp>
#include <demon/plugin.hpp>

#include <common/utils.hpp>

#include <sys/un.h>

class DesktopEnv;

// using in header?
using CastCondition = bool (*)();
using InplaceCast = void (*)( DesktopEnv* self );

struct CastRule {
    CastCondition cond;
    InplaceCast cast;
};

void registrateAll();

size_t sizeForDE();

DesktopEnv* initDE();

class DesktopEnv : public Plugin {
protected:
    void castToBase();

public:
    DesktopEnv( bool autoclearSocket );
    virtual ~DesktopEnv();

    virtual ProcessInfo getFocused();

    void OnTrigger() override;
    void checkDE();

    operator int() {
        return getFd();
    }
};

class _SwayDE final : public DesktopEnv {
    std::string getAnswer();

public:
    _SwayDE();
    ~_SwayDE();

    // functional

    ProcessInfo getFocused() override;

    // SOLID support functions

    static bool CastCondition();

    static void InplaceCast( DesktopEnv* self );
};

class _Hyprland final : public DesktopEnv {
    sockaddr_un addr{};


public:
    _Hyprland();
    ~_Hyprland();

    // functional

    ProcessInfo getFocused() override;

    // SOLID support functions

    static bool CastCondition();

    static void InplaceCast( DesktopEnv* self );
};

class _HyprlandTrue final : public DesktopEnv {
    _Hyprland _base;

public:
    _HyprlandTrue();
    ~_HyprlandTrue();

    // functional

    ProcessInfo getFocused() override;

    // SOLID support functions

    static bool CastCondition();

    static void InplaceCast( DesktopEnv* self );
};
