#include "src/dal/book.hpp"
#include "src/dal/book_shelf.hpp"
#include "src/commands/move_books.hpp"

extern void remove_book(const char *fileName);
namespace commands{
    
    void MoveBooks::execute() {
         soci::session sql_(dal::soci_session_factory(), true);
         dal::BookOnShelf db_book_on_shelf(sql_);
        const bl::BookShelf& BS_ = bs_;
        std::for_each(books_to_move_.begin(),
                      books_to_move_.end(),
                      [&db_book_on_shelf, BS_](const std::pair<std::string, bl::Book>& b){
                          db_book_on_shelf.del_book_records(b.second.id());
                          std::pair< bl::db_id, bl::db_id> book_on_shelf(b.second.id(),BS_.id());
                          db_book_on_shelf.insert(book_on_shelf);
                          
        });
    }
}

