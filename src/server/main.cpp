#include "common/logger.hpp"
#include "server/server.hpp"

Logger logger( LogLvl::Error );

// data send straigh here

// like in test task, i want to use HTTP REST and JSON

// i should contain on server jsons? 
// btw from client i can pass bit cast of size as in sway

int main() {
    const auto port = 15070;
    Server server( port );

    server.run();
}
