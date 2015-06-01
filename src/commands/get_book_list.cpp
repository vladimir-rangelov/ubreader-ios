#include "src/commands/get_book_list.hpp"


namespace commands{
    
    void GetBookList::execute() {
        dal::Book db_book(sql_);
        if(shelf_.id() != boost::uuids::nil_generator() ()){
            books_from_db_ = db_book.rows(search_,
                                          sort_order_,
                                          shelf_,
                                          belongs_to_shelf_);
        } else {
            books_from_db_ = db_book.rows(search_,sort_order_);
        }
    }
}

