#pragma once

#include "demon/settings.hpp"
#include "gui/db.hpp"
#include "gui/client.hpp"
#include "gui/state.hpp"
#include "gui/utils.hpp"

struct Context {
    DatabaseReader db;
    Client ipc;
    State state;
    Settings settings;

    Utils utils;
};

struct GContext {
    static Context* ctx;
};
