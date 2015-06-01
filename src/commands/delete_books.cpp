#include "src/commands/delete_books.hpp"

extern void remove_book(const char *fileName);
namespace commands{
    
    void DeleteBooks::execute() {
         soci::session sql_(dal::soci_session_factory(), true);
         dal::Book db_book(sql_);
        std::for_each(books_to_delete_.begin(), books_to_delete_.end(), [&db_book](const std::pair<std::string, bl::Book> && b){
            std::string file_name =b.second.file_name_;
            remove_book(file_name.c_str());
        });
    }
}

