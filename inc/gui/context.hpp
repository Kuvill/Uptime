#pragma once

#include "demon/settings.hpp"
#include "gui/db.hpp"
#include "gui/client.hpp"
#include "gui/state.hpp"
#include "gui/utils.hpp"

class Context {
    static Context* self;

    friend int main( int, char** );

public:
    DatabaseReader db;
    Client ipc;
    State state;
    Settings settings;

    Utils utils;

    static Context* get();
};
