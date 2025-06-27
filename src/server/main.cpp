#include "common/logger.hpp"
#include "server/server.hpp";

Logger logger( LogLvl::Error );

int main() {
    const auto port = "666";
    Server server( port );

    server.run();
}
