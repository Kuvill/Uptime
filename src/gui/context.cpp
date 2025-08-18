#include "gui/context.hpp"
#include "common/logger.hpp"
#include "common/time.hpp"
#include "glib.h"
#include "gui/record_item.hpp"

#include <algorithm>
#include <iterator>
#include <unordered_map>

using namespace std::chrono;

// OOP way: create state pattern class, that remember state.
// more cheap way: create static function getDif
    void State::changeModel( Duration ) {
        logger.log(LogLvl::Info, "Changing view model");

        // looks really ugly
        // only way that i see - use variant over enum 
        // but will it appearance?
        switch (_bound.model) {
            case Duration::ByYear:
                _bound.from = duration_cast<years>( getCurrentTime() );
                _bound.to = _bound.from + years(1);
                break;

            case Duration::ByMonth:
                _bound.from = duration_cast<months>( getCurrentTime() );
                _bound.to = _bound.from + months(1);
                break;

            case Duration::ByDay: _bound.from = duration_cast<days>( getCurrentTime() ); _bound.to = _bound.from + days(1);
                break;
                
            case Duration::All:
                _bound.from = recTime_t::min();
                _bound.to = recTime_t::max();
                break;
        }
    }

    void State::next() {
        switch (_bound.model) {
            case Duration::ByYear:
                _bound.to = _bound.from + years(1);
                break;

            case Duration::ByMonth:
                _bound.to = _bound.from + months(1);
                break;

            case Duration::ByDay:
                _bound.to = _bound.from + days(1);
                break;
                
            case Duration::All:
                break;
        }
    }
    void State::prev() {
        switch (_bound.model) {
            case Duration::ByYear:
                _bound.to = _bound.from - years(1);
                break;

            case Duration::ByMonth:
                _bound.to = _bound.from - months(1);
                break;

            case Duration::ByDay:
                _bound.to = _bound.from - days(1);
                break;
                
            case Duration::All:
                break;
        }
    }

    void State::setFrom() {

    }
    void State::setTo() {

    }

    // mb resive in first pointer will be greater
    void State::mergeStore( std::tuple<RawRecordItem**, int> items ) {
        g_list_store_splice(_store, 0, 0, (void**)(std::get<0>(items)), std::get<1>(items));
        g_list_store_sort( _store, RecordItemUptimeCompare , nullptr );

        // here i delete 2 pointers. Not items
        delete std::get<0>(items);
    }

namespace {
    struct strEqual {
        bool operator()( gchar* lhs, gchar* rhs ) const {
            return g_strcmp0(lhs, rhs) == 0;
        }
    };

    struct hashGstr {
        unsigned long operator()( gchar* str ) const {
            unsigned long hash = 5381;
            int c;

            while ((c = *str++))
                hash = ((hash << 5) + hash) + c;

            return hash;

        }
    };
}

    /* 
       What, do i need uptime?) mb for guards but idk

       There is 3 situations:
       1. Contigous usage of one app.
       2. Switch focus to other app.
       3. Same app after poweroff. (or filler since i haven't this one)
   */
    //  FIXME Very blowed
    void State::mergeStoreRightVersion( std::tuple<RawRecordItem**, int> items ) {
        std::unordered_map<gchar*, recTime_t, hashGstr, strEqual> result;

        RawRecordItem** records = std::get<0>( items );
        int count = std::get<1>( items );

        // when after app launch it opened first time
        // should use bool over -1?
        recTime_t startUse = records[0]->recTimer;

        for( int i = 0; i < count - 1; ++i ) {
            auto item = records[i];
            auto nextItem = records[i+1];

            if( item->appName == nextItem->appName ) {

                // 1.
                if( nextItem->recTimer - item->recTimer <= 5s + 5s/2 ) {
                    continue;
                    
                // 3
                } else {
                    result[item->appName] += item->recTimer - startUse + 5s;
                    startUse = nextItem->recTimer;
                }

            } else {
                // 2.
                result[item->appName] += item->recTimer - startUse + 5s;
                startUse = nextItem->recTimer;
}
        }

        auto item = records[count-1];
        result[item->appName] += item->recTimer - startUse + 5s;

        RecordItem** store = new RecordItem*[result.size()];
        logger.log(LogLvl::Info, result.size(), " unique applications loaded");

        auto elem = result.begin();
        for( int i = 0; i < result.size(); ++i, ++elem ) {
            if( elem->first == nullptr ) logger.log(LogLvl::Warning, i);
            store[i] = record_item_new( g_strdup( elem->first), elem->second.count() );
        }

        delete[] records;

        g_list_store_splice(_store, 0, 0, (gpointer*)store, result.size());
}

    /*
    void State::mergeStoreUnique( std::tuple<RecordItem**, int> items ) {
        std::sort( std::get<0>(items), std::get<0>(items) + std::get<1>(items), RecordItemNameLess );

        auto end = std::unique(  std::get<0>(items), std::get<0>(items) + std::get<1>(items),
                []( RecordItem* lhs, RecordItem* rhs ){
                    if( lhs->appName == rhs->appName ) {
                        // FIXME i have to compare with context->cd
                        if( lhs->uptime - rhs->uptime > 5s )
                            lhs->uptime += rhs->uptime;

                        return false;
                    }

                    return true;
                } );

        int endPos = end - std::get<0>( items );

        g_list_store_splice(_store, 0, 0, (void**)(std::get<0>(items)), endPos);

        // here i delete 2 pointers. Not items
        delete std::get<0>(items);
    }
*/

    void State::setStore( GListStore* store ) {
        _store = store;
    }

    GListStore* State::getStore() { return _store; }


void State::createTimer() {
    _timer = g_timer_new();
}
