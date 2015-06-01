#include "src/commands/delete_shelf.hpp"


namespace commands{
    
    void DeleteShelf::execute() {
        soci::session sql_(dal::soci_session_factory(), true);
        dal::BookShelf db_book_shelf(sql_);
        db_book_shelf.base_functions_.del(shelf_to_delete_.id());
    }
    
}

