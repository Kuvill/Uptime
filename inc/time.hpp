#pragma once

#include <chrono>
#include <concepts>

// seconds. MUST BE time since unix epoch
using recTime_t = std::chrono::seconds;

recTime_t getCurrentTime();

template< typename T >
concept integral = std::is_integral_v<T>;
recTime_t toRecTime( integral auto number ) {
    return static_cast<recTime_t>( number );
}
