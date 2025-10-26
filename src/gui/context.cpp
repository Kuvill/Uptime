#include <gui/context.hpp>

static const char* dbName = "uptime.db";

Context* Context::self = nullptr;

Context* Context::get() {
    if( !self )
        self = new Context {
            DatabaseReader( dbName ),
            Client(),
            State(),
            /*Settings()*/ };

    return self;
}

