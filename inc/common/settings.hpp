#pragma once

#include "common/toml_structs.hpp"
#include <filesystem>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

[[maybe_unused]] constexpr inline const std::string_view CONF_PATH = ".config/uptimer/";
[[maybe_unused]] constexpr inline const std::string_view CACHE_PATH = ".cache/uptimer/";
[[maybe_unused]] constexpr inline const std::string_view SHARE_PATH = ".local/share/uptimer/";
[[maybe_unused]] constexpr inline const std::string_view STATE_PATH = ".local/state/uptimer/";

[[maybe_unused]] constexpr inline const std::string_view PATH_LABEL = "paths";
[[maybe_unused]] constexpr inline const std::string_view GENERAL_LABEL = "general";
[[maybe_unused]] constexpr inline const std::string_view DEMON_LABEL = "demon";
[[maybe_unused]] constexpr inline const std::string_view GUI_LABEL = "gui";

class Settings {
    // i want to free _conf after an time
    // let user cache an needed value and add callback
    toml::table _conf;
protected:
    std::filesystem::path _path = CONF_PATH;

    // use libfrozen for consteval idle hash map

    // Anyway i don't like coast of this things
    // Those abstrations are to expensive
    // But i like it more, then just update an value by reference
    std::unordered_map<TomlId, Callbacks,
                        TomlIdHash, TomlIdEq> map;

    bool try_init();

public:

    Settings();

    Settings( std::string_view path );

    ~Settings();

    void add( TomlId&& id, Callbacks&& cbs );

    void call( TomlIdView id, toml::node& node );

    void scan();

    template< typename T >
    std::optional<T> value( TomlIdView& view ) {
        return _conf[view.label][view.variable].value<T>(); 
    }

    template< typename T >
    T value_or( TomlIdView view, T default_val ) {
        return _conf[view.label][view.variable].value_or( default_val ); 
    }
};

extern Settings *settings_;
