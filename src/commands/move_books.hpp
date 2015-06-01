#ifndef commands_MoveBooks_included_hpp
#define commands_MoveBooks_included_hpp

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

#include "src/bl/book.hpp"
#include "src/bl/book_shelf.hpp"


namespace commands{
    
struct MoveBooks {
    typedef void result_type;
    const std::unordered_map<std::string, bl::Book>& books_to_move_;
    const bl::BookShelf& bs_;
    MoveBooks(
        std::unordered_map<std::string, bl::Book>& books_to_move,
              const bl::BookShelf& bs) : books_to_move_(books_to_move), bs_(bs){ }
   
    result_type execute( );

};

}

#endif
