#ifndef dal_BookAttributes_hpp
#define dal_BookAttributes_hpp

#include <string>
#include "src/bl/book_attributes.hpp"
#include "src/bl/book.hpp"
#include "src/dal/base.hpp"
#include "src/dal/id.hpp"

namespace dal {
  template<typename T> class base;
}


namespace dal{

struct BookAttributes {
    BookAttributes(soci::session& sql, std::string table_name) :
      sql_(sql), table_name_(table_name) {
  }
  
  void insert(const bl::BookAttributes& ba) {
        const std::string& str_id = convert::id(dal::id::get(ba.book_));
        soci::blob b(sql_);
        std::size_t length = ba.raw_data_.size();
        b.write(0, ba.raw_data_.data(), length);

        sql_ << "insert into "<< table_name_
            << " (book_id, raw_data) values(:id, ?)",soci::use(str_id), soci::use(b);

  }
    void update_if_exist_else_insert(const bl::BookAttributes& ba) {
        try {
            insert(ba);
        } catch(soci::soci_error e){
            update(ba);
        }
        
    }
  void update(const bl::BookAttributes& ba) {
        const std::string& str_id = convert::id(dal::id::get(ba.book_));
        soci::blob b(sql_);
      
        std::size_t length = ba.raw_data_.size();
        b.write(0, ba.raw_data_.data(), length);
        
        sql_ << "update "<< table_name_
        << "  set raw_data=? where book_id = :id", soci::use(b), soci::use(str_id);
    }
  
  template<typename T>
  void get( T& ba, const bl::db_id& book) {
          const std::string& str_id = convert::id(dal::id::get(book));
          soci::blob b(sql_);
          
          sql_ << "select raw_data from "<< table_name_
            << " where book_id = :id", soci::into(b), soci::use(str_id);
          
          ba.resize_raw_data(b.get_len());
          b.read(0, ba.raw_ptr(), b.get_len());
  }

    void del(boost::uuids::uuid id) {
        const std::string& str_id = convert::id(id);
        sql_ << "DELETE FROM "<< table_name_<<" WHERE book_id=:id",
        soci::use(str_id);
    }
    int count() {
        int c = 0;
        sql_ << "SELECT count(*) FROM " << table_name_,
        soci::into(c);
        return c;
    }
private:
    soci::session& sql_;
    std::string table_name_;
  };

}
#endif
