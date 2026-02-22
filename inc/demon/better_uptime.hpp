#pragma once

#include <common/aliases.hpp>
#include <common/utils.hpp>
#include <demon/process_info.hpp>

#include <sdbus-c++/IConnection.h>

#include <sys/un.h>

class DesktopEnv;

// using in header?
namespace Details__ {
    using CastCondition = bool (*)();
    using InplaceCast = void (*)( DesktopEnv* self );
}

struct CastRule {
    Details__::CastCondition cond;
    Details__::InplaceCast cast;
};

void registrateAll();

size_t sizeForDE();

DesktopEnv* initDE();

class DesktopEnv {
protected:
    int _fd; // on fd change it have to trigger poll
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

class _Gnome final : public DesktopEnv {
    std::unique_ptr<sdbus::IConnection> _connection;
    std::unique_ptr<ProcessInfo> _processInfo; // idk how to do not store it all time ( 

    void TryStartExtension();

    void renewPollData();

    void dbusCallback( sdbus::Signal );
public:
    _Gnome();
    ~_Gnome() = default;

    ProcessInfo getFocused() override;

    static bool CastCondition();

    static void InplaceCast( DesktopEnv* self );
};
