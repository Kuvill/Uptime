#include "common/time.hpp"
#include <chrono>

recTime_t getCurrentTime() {
    return std::chrono::duration_cast<recTime_t>( std::chrono::system_clock::now().time_since_epoch() );
}
