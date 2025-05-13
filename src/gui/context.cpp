#include "inc/context.hpp"
#include <algorithm>
#include "inc/logger.hpp"

using namespace std::chrono;

// OOP way: create state pattern class, that remember state.
// more cheap way: create static function getDif
    void State::changeModel( Duration ) {
        logger.log(LogLvl::Info, "Changing view model");

        // looks really ugly
        // only way that i see - use variant over enum 
        // performance the same ( 4-8 byte loose. but i have only once instance)
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
    void State::mergeStore( std::tuple<RecordItem**, int> items ) {
        g_list_store_splice(_store, 0, 0, (void**)(std::get<0>(items)), std::get<1>(items));
        g_list_store_sort( _store, RecordItemUptimeCompare , nullptr );

        // here i delete 2 pointers. Not items
        delete std::get<0>(items);
    }

    // not so fast. Have to change Model to improve performance
    void State::mergeStoreUnique( std::tuple<RecordItem**, int> items ) {
        std::sort( std::get<0>(items), std::get<0>(items) + std::get<1>(items), RecordItemNameGrater );

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
    void State::setStore( GListStore* store ) {
        _store = store;
    }

    GListStore* State::getStore() { return _store; }


