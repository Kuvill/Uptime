#include <cstdlib>
#include <thread>

#include <common/logger.hpp>

// for DE i should create one more function, 
// that transfer from subscribe model to cooldown
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
