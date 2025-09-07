#include <cstdlib>
#include <thread>

#include <common/logger.hpp>

char* blockingGetEnv( const char* var ) {
    char* env = std::getenv( var );
 
    if( !env ) [[unlikely]] {
        logger.log(LogLvl::Warning, "Unable to access ENV (", var, "). Let it load...");
        std::this_thread::sleep_for( std::chrono::seconds(4) );

        env = std::getenv( var );

        if( !env ) {
            logger.log(LogLvl::Error, "Unable to access ENV(", var, ")!");
            throw std::runtime_error("Unable to access ENV");
        }
    }

    return env;
}
