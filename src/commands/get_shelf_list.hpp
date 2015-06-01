#ifndef commands_GetShelfList_included_hpp
#define commands_GetShelfList_included_hpp

#include <string>
#include <vector>
#include <utility>

#include "src/dal/book_shelf.hpp"

namespace commands{
    
struct GetShelfList {
    typedef void result_type;
    soci::session& sql_;
    std::vector<bl::BookShelf> shelfs_from_db_;

    GetShelfList(soci::session& sql) : sql_(sql) {
    }
   
    result_type execute( );

};

}

#endif
