#pragma once

#include "demon/get_uptime.hpp"
#include <common/aliases.hpp>

#include <sys/un.h>

class DesktopEnv;

// using in header?
using CastCondition = bool (*)( std::string_view env );
using InplaceCast = void (*)( DesktopEnv* self );

struct CastRule {
    CastCondition cond;
    InplaceCast cast;
};

ulong DE_maxSize();

class DesktopEnv {
protected:
    virtual void castToBase();

public:
    DesktopEnv() = default;
    virtual ~DesktopEnv() = default;

    // functional

    virtual ProcessInfo getFocused();

    virtual DesktopEnv* checkDE();
    
    // SOLID support functions

    virtual ulong getSizeof();

    [[noreturn]] virtual CastRule returnCastRule() const;
};

class _SwayDE final : public DesktopEnv {
    int _sock;

    void castToBase() override;

public:
    _SwayDE();
    ~_SwayDE();

    // functional

    ProcessInfo getFocused() override;

    DesktopEnv* checkDE() override;

    // SOLID support functions

    ulong getSizeof() override;

    virtual CastRule returnCastRule() const override;
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

    DesktopEnv* checkDE() override;

    // SOLID support functions

    ulong getSizeof() override;

    virtual CastRule returnCastRule() const override;
};
