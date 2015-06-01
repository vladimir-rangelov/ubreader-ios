#ifndef commands_UpdateShelf_included_hpp
#define commands_UpdateShelf_included_hpp

#include "src/dal/book_shelf.hpp"

namespace commands{
    struct UpdateShelf{
        typedef void result_type;
        soci::session& sql_;
        const bl::BookShelf& shelf_to_update_;
        UpdateShelf(soci::session& sql,const bl::BookShelf& shelf_to_update) : sql_(sql),
                shelf_to_update_(shelf_to_update) {
        }
        
        result_type execute( );
    };

}


#endif
