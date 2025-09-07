// call std::getenv. if result is null recall std::getenv.
// if it still null, throw runtime
char* blockingGetEnv( const char* var );
