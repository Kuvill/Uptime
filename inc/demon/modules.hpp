#pragma once 

#include <unordered_map>

class Modules {
    using Callback = void(*)();
    using Map = std::unordered_map<int, Callback>;
    std::unordered_map<int, Callback> _modules;

public:
    Modules(){}

    void add( int fd, Callback cb ) {
        _modules.insert( {fd, cb} );
    }

    void trigger( int fd ) {
        _modules[fd]();
    }

    Map::iterator begin() {
        return _modules.begin();
    }

    Map::iterator end() {
        return _modules.end();
    }
};
