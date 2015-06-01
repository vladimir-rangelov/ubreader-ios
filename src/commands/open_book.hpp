#ifndef commands_OpenBook_included_hpp
#define commands_OpenBook_included_hpp

#include "src/bl/book.hpp"
#include "src/dal/base.hpp"

namespace commands{
    struct OpenBook{
        typedef void result_type;
        bl::Book& book_to_open_;
        OpenBook(bl::Book& b) : book_to_open_(b) { }
        result_type execute();
    };

}


#endif
