#include "src/commands/update_shelf.hpp"


namespace commands{
    
    void UpdateShelf::execute() {
       
        dal::BookShelf db_book_shelf(sql_);
        db_book_shelf.update(shelf_to_update_);
    }
    
}

