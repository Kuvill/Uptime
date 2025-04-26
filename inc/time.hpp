#pragma once

#include <chrono>

// seconds. MUST BE time since unix epoch
using recTime_t = std::chrono::seconds;

recTime_t getCurrentTime();
