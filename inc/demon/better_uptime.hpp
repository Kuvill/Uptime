#pragma once

#include "demon/get_uptime.hpp"
#include <common/aliases.hpp>

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

class DesktopEnv {
protected:
    int _fd;
    void castToBase();

public:
    DesktopEnv();
    virtual ~DesktopEnv();

    virtual ProcessInfo getFocused();

    virtual void OnTrigger();
    void checkDE();

    operator int() {
        return _fd;
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
