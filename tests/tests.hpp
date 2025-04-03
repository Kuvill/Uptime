#pragma once

#include "inc/logger.hpp"
#include "inc/db.hpp"
#include "inc/ram_storage.hpp"
#include "inc/server.hpp"

void run();

bool checkDB();

bool checkRam();

bool checkServer();
