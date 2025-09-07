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
    virtual void castToBase();

public:
    DesktopEnv() = default;
    virtual ~DesktopEnv() = default;

    virtual ProcessInfo getFocused();

    void checkDE();
};

class _SwayDE final : public DesktopEnv {
    int _sock;

    void castToBase() override;

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
    int _sock;
    sockaddr_un addr{};

    void castToBase() override;

public:
    _Hyprland();
    ~_Hyprland();

    // functional

    ProcessInfo getFocused() override;

    // SOLID support functions

    static bool CastCondition();

    static void InplaceCast( DesktopEnv* self );
};
