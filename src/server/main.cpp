#include "common/logger.hpp"
#include "server/server.hpp";

Logger logger( LogLvl::Error );

int main() {
    const int port = 666;
    Server server;

    server.run();
}
