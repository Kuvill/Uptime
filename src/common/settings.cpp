#include <common/settings.hpp>

#include "common/logger.hpp"
#include "common/toml_structs.hpp"
#include <filesystem>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

[[maybe_unused]] constexpr static const std::string_view CONF_DEF_PATH = ".config/uptimer/";

bool Settings::try_init() {
    auto result = toml::parse_file( _path.c_str() ); // _path.string - will return new allocated string
    logger.log( LogLvl::Info, "Processing config file: ", _path );

    if( !result ) {
        logger.log(LogLvl::Error, "Parse error! ", result.error().description() );
        return false;
    } _conf = std::move( result.table() );

    return true;
}

Settings::Settings( std::string_view path ) : _path( path ) {
    try_init();
}

Settings::Settings() : Settings( CONF_DEF_PATH ){
}

Settings::~Settings() = default;

void Settings::add( TomlId&& id, Callbacks&& cbs ){ 
    map[id] = cbs;
}

void Settings::call( TomlIdView id, toml::node& node ) {
    auto it = map.find( id );
    if( it == map.end() ){
        logger.log(LogLvl::Error, "Unknown settings parameter!");
        return;
    }

    it->second.on_find( node );
    it->second.found = true;
}

void Settings::scan() {
    for( auto& a : map ) 
        a.second.found = false; // cache unfriendly

    if( !try_init() )
        goto SET_DEFAULT_VALUES; // unnecessary goto FIXME


    // std::visit monkaS
    _conf.for_each( [this]( auto& key, auto& value ) {
        logger.log( LogLvl::Info, "Processing label: [{}]", key.str() );

        if constexpr( !toml::is_table<decltype(value)> ) {
            logger.log(LogLvl::Error, "Dangling variables found! Skipping it..."); // get line number or at least var name
            return;
        }

        for( auto& field : *value.as_table() ) {
            call( {key.str(), field.first.str()}, field.second );
        }
    });

SET_DEFAULT_VALUES:

    for( auto& a : map ) {
        if( !a.second.found ) {
            std::println("Callling on_absence for {}/{}", a.first.label, a.first.variable);
            a.second.on_absence();
        }
    }
}

Settings* settings_;
