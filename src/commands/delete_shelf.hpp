#ifndef commands_DeleteShelf_included_hpp
#define commands_DeleteShelf_included_hpp

#include "src/dal/book_shelf.hpp"

namespace commands{
    struct DeleteShelf{
        typedef void result_type;
        bl::BookShelf& shelf_to_delete_;
        DeleteShelf(bl::BookShelf& shelf_to_delete) :
            shelf_to_delete_(shelf_to_delete) {
        }
        
        result_type execute( );
    };
    
}

#endif
