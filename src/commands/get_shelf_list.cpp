#include "src/commands/get_shelf_list.hpp"


namespace commands{
    
    void GetShelfList::execute() {
        dal::BookShelf db_book_shelf(sql_);
        shelfs_from_db_ = db_book_shelf.sorted_rows();
    }
}

