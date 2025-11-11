#pragma once

#include <string>
#include <string_view>
#include <functional>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

struct TomlId {
    std::string label;
    std::string variable;
};

struct TomlIdView {
    std::string_view label;
    std::string_view variable;
};

struct TomlIdHash {
    using is_transparent = void;

    std::size_t operator()( const TomlId& id ) const {
        return std::hash<std::string_view>()( id.label ) ^ std::hash<std::string_view>()( id.variable );
    }

    std::size_t operator()( const TomlIdView& id ) const {
        return std::hash<std::string_view>()( id.label ) ^ std::hash<std::string_view>()( id.variable );
    }
};

struct TomlIdEq {
    using is_transparent = void;

    bool operator()( const TomlId& lhs, const TomlId& rhs ) const {
        return lhs.label == rhs.label && lhs.variable == rhs.variable;
    }

    bool operator()( const TomlIdView& lhs, const TomlId& rhs ) const {
        return lhs.label == rhs.label && lhs.variable == rhs.variable;
    }

    bool operator()( const TomlId& lhs, const TomlIdView& rhs ) const {
        return lhs.label == rhs.label && lhs.variable == rhs.variable;
    }

    bool operator()( const TomlIdView& lhs, const TomlIdView& rhs ) const {
        return lhs.label == rhs.label && lhs.variable == rhs.variable;
    }
};

struct Callbacks {
    std::function<void(toml::node&)> on_find;
    std::function<void()> on_absence;

    Callbacks() = default;
    Callbacks( std::function<void(toml::node&)> _on_find, std::function<void()> _on_absence ) : on_find(_on_find), on_absence(_on_absence ){}
    Callbacks( const Callbacks& other ) : on_find( other.on_find ), on_absence( other.on_absence ) {}
    Callbacks( Callbacks&& other ) : on_find( other.on_find ), on_absence( other.on_absence ) {}

    Callbacks& operator=( Callbacks&& other ) = default;
    Callbacks& operator=( const Callbacks& other ) = default;

    // class with private Aga
private:
    bool found = false;
    friend class Settings;
};
