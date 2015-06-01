#include "src/commands/import_new_books.hpp"
#include "src/dal/book.hpp"
#include "src/dal/book_cover_attribute.hpp"

namespace commands{
    
    bool ImportNewBooks::already_imported(const std::string book_file_name ) const{
        
        for(std::list<bl::Book>::const_iterator it = books_from_db_.begin();
            it != books_from_db_.end(); it++){
            if(it->file_name_ == book_file_name) {
                return true;
            }
        }
        
        return false;
    }
    
  
    
    void ImportNewBooks::delete_removed_books() {
        
        for (auto j = books_from_db_.begin(); j != books_from_db_.end(); j++) {
            bool flag_remove = true;
            for (int i = 0; i < source_.count(); i++) {
                
                if (j->file_name_ == source_.get_book_file_name(i)) {
                    flag_remove = false;
                    break;
                }
            }
            if (flag_remove) {
                soci::session sql_(dal::soci_session_factory(), true);
                dal::Book db_book(sql_);
                dal::BookCoverAttribute db_book_cover_att(sql_);
                dal::BookCoverCacheAttribute db_book_cover_cache_att(sql_);
                sql_.begin();
                try {
                    db_book.base_functions_.del(j->id());
                    db_book_cover_att.del(j->id());
                    db_book_cover_cache_att.del(j->id());
                 }
                 catch (...) {
                     sql_.rollback();
                     continue;
                }
                sql_.commit();
            }
        }
    }
    
    ImportNewBooks::result_type ImportNewBooks::execute() {

        ImportNewBooks::result_type rez = 0;
        {
            soci::session sql(dal::soci_session_factory());
            dal::Book db_book(sql);
            books_from_db_ = db_book.base_functions_.rows();
        }
        
        delete_removed_books();
        
        for (int i = 0; i < source_.count(); i++) {

            if ( !already_imported(source_.get_book_file_name(i)) ) {
                std::tuple<bl::Book, std::vector<char>,
                std::vector<char> > b;
                try {
                    b = source_.get_book(i);
                } catch (std::runtime_error e) {
                    std::cout << "import error - "
                              << e.what()
                              << " in book:"
                              << source_.get_book_file_name(i);
                    continue;
                }
                
                soci::session sql_(dal::soci_session_factory(), true);
                dal::Book db_book(sql_);
                dal::BookCoverAttribute db_book_cover_att(sql_);
                dal::BookCoverCacheAttribute db_book_cover_cache_att(sql_);
                sql_.begin();
                try {
                db_book.insert(std::get<0>(b));
                bl::BookAttributes ba;
                ba.raw_data_ = std::get<1>(b);
                ba.book_ = std::get<0>(b);
                db_book_cover_att.insert(ba);
                if (std::get<1>(b).size() != 0) {
                    bl::BookAttributes cover;
                    cover.raw_data_ = std::get<1>(b);
                    cover.book_ = std::get<0>(b);
                    db_book_cover_att.update_if_exist_else_insert(cover);
                    
                    cover.raw_data_ = std::get<2>(b);
                    db_book_cover_cache_att.update_if_exist_else_insert(cover);
                    
                }
                } catch (...) {
                    sql_.rollback();
                    continue;
                }
                sql_.commit();
                rez++;
            }
        }
        return rez;
    }
}

