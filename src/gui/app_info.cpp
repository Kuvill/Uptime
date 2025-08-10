#include "gui/app_info.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

// home smaller so i will search here first
static const char* APP_DIRS[] {
     "~/.local/share/applications/",
     "/usr/share/applications/",
};

struct CategoriesInfo {
    const char* str;
    int SepPos;
};

DesktopEntryInfo::DesktopEntryInfo( std::string&& info, std::string::size_type semiPos )
        : _info( info ), semicolonPos( semiPos ) {}

std::string_view DesktopEntryInfo::getComment() {
    return { _info.c_str(), semicolonPos };
}

std::string_view DesktopEntryInfo::getIcon() {
    return { _info.c_str(), _info.size() - semicolonPos };
}

// beaty solve - second param - iter, that init in constexpr
static CategoriesInfo COMMENT_CAT = {"Comment", -1}; 
static CategoriesInfo ICON_CAT = {"Icon", -1}; 
static CategoriesInfo ComAndIco_CAT = {"CommentIcon", 7}; 

enum class Category {
    Comment,
    Icon,
    CommentAndIcon
};

// bad naming btw. Confuse with enum
class Categories {
    CategoriesInfo _info;

public:
    Categories( Category cat ) {
        if( cat == Category::Comment )
            _info = COMMENT_CAT;
        else if( cat == Category::Icon )
            _info = ICON_CAT;
        else
            _info = ComAndIco_CAT;
    }

private:
    struct It {
        const char* ptr;

        It( const char* ptr ) : ptr(ptr) {}

        void operator+=( int val ) { ptr += val; }
        const char* operator++(){  return ++ptr; }
        const char* operator++( int ){ const char* old = ptr--; return old; }

        void operator-=( int val ) { ptr -= val; }
        const char* operator--(){ return --ptr; }
        const char* operator--( int ){ const char* old = ptr--; return old; }

        char operator*(){ return *ptr; }

        bool operator==( It other ) {
            if( ptr == nullptr || other.ptr == nullptr )
                return true;

            return ptr == other.ptr;
        }
    };

public:
    It begin() const {
        return _info.str;
    }

    It end() const {
        if( _info.SepPos == -1 )
            return nullptr;

        return _info.str + _info.SepPos;
    }

};

// return empty string if no entries
fs::path AppInfo::getDesktopEntryPath( std::string_view appName ) {

    for( auto path : paths ) {
        std::string searchedPath = path; searchedPath.append( appName );

        for( const auto& entry : fs::recursive_directory_iterator( path) ) {
            if( entry.is_regular_file() ) {

                if( entry.path() == searchedPath ) {
                    return entry.path();
                }
            }
        }
    }

    return std::string();
}

AppInfo::AppInfo() {
    for( auto path : APP_DIRS ) {
        if( fs::exists( path ))   
            paths.emplace_back( path );
    }
}

AppInfo::AppInfo( std::string_view additionalPath ) : AppInfo() {
    paths.emplace_back( additionalPath );
}

// TODO
DesktopEntryInfo AppInfo::getDesktopEntryInfo( std::string_view appName ) {
    std::ifstream file( getDesktopEntryPath( appName ) );

    std::istream_iterator<char> begin( file ), end;

    // should reserve space
    std::string f = { begin, end };
    auto it = ++std::find( f.begin(), f.end(), '\n' ); // skip [Desktop Entry]

    while( it != f.end() ) {

        auto end = std::find( it, f.end(), '=' );
        std::string_view category = {it, end};

        // since there is no std::find_first_not_of nor method with iterator support
        // i won't care about spaces
        if( category == "Comment" ) {
            auto comEnd = std::find( ++end, f.end(), '\n' );
            return {{end, comEnd}, 1};
        }

        else if( category == "Icon" ) {

        }

        it = ++std::find(it+1, f.end(), '\n');
    } 

    return {std::string(), 0};
}

std::string AppInfo::getComment( std::string_view appName ) {
    std::ifstream file( getDesktopEntryPath( appName ) );

    std::istream_iterator<char> begin( file ), end;

    // should reserve space
    std::string f = { begin, end };
    auto it = ++std::find( f.begin(), f.end(), '\n' ); // skip [Desktop Entry]

    while( it != f.end() ) {

        auto end = std::find( it, f.end(), '=' );
        std::string_view category = {it, end};

        // since there is no std::find_first_not_of nor method with iterator support
        // i won't care about spaces
        if( category == "Comment" ) {
            auto comEnd = std::find( ++end, f.end(), '\n' );
            return {end, comEnd};
        }

        it = ++std::find(it+1, f.end(), '\n');
    } 

    return std::string();
}

fs::path AppInfo::getIconPath( std::string_view appName ) {
    std::ifstream file( getDesktopEntryPath( appName ) );

    std::istream_iterator<char> begin( file ), end;

    std::string f = { begin, end };
    auto it = ++std::find( f.begin(), f.end(), '\n' );

    while( it != f.end() ) {

        auto end = std::find( it, f.end(), '=' );
        std::string_view category = {it, end};

        if( category == "Icon" ) {
            auto comEnd = std::find( ++end, f.end(), '\n' );
            return {end, comEnd};
        }

        it = ++std::find(it+1, f.end(), '\n');
    } 

    return std::string();
}
