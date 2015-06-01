#ifndef commands_AddNewShelf_included_hpp
#define commands_AddNewShelf_included_hpp

#include "src/dal/book_shelf.hpp"

namespace commands{
    struct AddNewShelf{
        typedef void result_type;
        bl::BookShelf& shelf_to_add_;
        AddNewShelf(bl::BookShelf& shelf_to_add) :
            shelf_to_add_(shelf_to_add) {
        }
        
        result_type execute( );
    };
    
}

#endif
