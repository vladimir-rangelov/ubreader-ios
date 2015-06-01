#include "src/commands/add_new_shelf.hpp"


namespace commands{
    
    void AddNewShelf::execute() {
        soci::session sql_(dal::soci_session_factory(), true);
        dal::BookShelf db_book_shelf(sql_);
        db_book_shelf.insert(shelf_to_add_);
    }
    
}

