#include "demon/settings.hpp"
#include <gui/context.hpp>

static const char* dbName = "uptime.db";

Context* Context::self = nullptr;

Context* Context::get() {
    if( !self ) {
        std::string path = settings_->paths.db;
        path += dbName;

        self = new Context {
            DatabaseReader( path.c_str() ),
            Client(),
            State(),
            /*Settings()*/ };
    }

    return self;
}

