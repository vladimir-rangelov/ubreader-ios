#include <gtest/gtest.h>

#include "src/details/utils.hpp"
#include "src/dal/Book.hpp"
#include "src/commands/import_new_books.hpp"
#include "src/dal/book_cover_attribute.hpp"

struct FakeBookSource : public commands::BookSourceBase {
    virtual int count()const{ return 3;}
    virtual std::string get_title(int index)const{
        switch (index) {
            case 0:
                return "BookTitle1";
            case 1:
                return "BookTitle2";
            case 2:
                return "BookTitle3";
        }
        return "";
    }
    
    virtual std::string get_book_file_name(int index)const    {
        
        switch (index) {
        case 0:
            return "BookPath1";
        case 1:
            return "BookPath2";
        case 2:
            return "BookPath13";
        }
        return "";
    }
    std::vector<char> get_cover(int index)const{
        
        switch (index) {
            case 0:
                return {'a','b','1'};
            case 1:
                return {'a','b','2'};
            case 2:
                return {'a','b','3'};
        }
        return {'a','b','9'};
    }
    
    std::tuple<bl::Book, std::vector<char>,
    std::vector<char> >  get_book(int index)const {
        std::tuple<bl::Book, std::vector<char>,
        std::vector<char> >  result;
        std::get<0>(result).title_ = get_title(index);
        std::get<0>(result).file_name_ = get_book_file_name(index);
        std::get<1>(result) = get_cover(index);
        std::get<2>(result) = get_cover(index);
        return result;
    }
};


TEST(commands, import_new_books) {


  FakeBookSource fs;
  commands::ImportNewBooks test_command(fs);
  test_command.execute();
    
  soci::session sql(dal::soci_session_factory());
    //sql.begin();
  dal::Book dal_book(sql);
  dal::BookCoverAttribute db_book_cover_att(sql);
    
  EXPECT_EQ(3,dal_book.base_functions_.count());
  EXPECT_EQ(3,db_book_cover_att.count());
    
      test_command.execute();
  EXPECT_EQ(3,dal_book.base_functions_.count());
  EXPECT_EQ(3,db_book_cover_att.count());
 // sql.rollback();
     dal::drop_database(sql);
     dal::create_all_tables(sql);
}

