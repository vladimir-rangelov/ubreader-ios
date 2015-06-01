#include "src/commands/open_book.hpp"
#include "src/dal/book.hpp"

namespace commands{
    
    void OpenBook::execute() {
        book_to_open_.last_read_ = pt::second_clock::local_time();
        soci::session sql_(dal::soci_session_factory(), true);
        dal::Book db_book(sql_);
        db_book.update(book_to_open_);
    }
    
}

