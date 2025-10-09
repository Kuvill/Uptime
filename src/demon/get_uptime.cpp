#include "demon/get_uptime.hpp"

#include <ostream>


std::ostream& operator<<( std::ostream& os, const ProcessInfo& info ) {
    os << "{'" << (info.name.data() ? info.name.data() : "") <<
        "' (" << info.describe << "), " << info.timestomp;

    return os;
}
