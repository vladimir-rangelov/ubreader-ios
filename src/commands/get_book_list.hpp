#ifndef commands_GetBookList_included_hpp
#define commands_GetBookList_included_hpp

#include <string>
#include <vector>
#include <utility>

#include "src/dal/book.hpp"

namespace commands{
    
struct GetBookList {
    typedef void result_type;
    soci::session& sql_;
    std::string search_;
    dal::Book::SortOrder sort_order_;
    std::vector<bl::Book> books_from_db_;
    bl::db_id shelf_;
    bool belongs_to_shelf_;
    GetBookList(soci::session& sql,
                std::string search,
                dal::Book::SortOrder sort_order,
                bl::db_id shelf,
                bool belongs_to_shelf) : sql_(sql), search_(search), sort_order_(sort_order), shelf_(shelf), belongs_to_shelf_(belongs_to_shelf)  {
    }
   
    result_type execute( );

};

}

#endif
