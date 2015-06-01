#include <gtest/gtest.h>

#include "src/details/utils.hpp"
#include "src/dal/Book.hpp"

void book_factory(bl::Book& unit_to_add);

void book_factory_second(bl::Book& unit_to_add);

TEST(sql_Book_list, simple_list) {

  soci::session sql(dal::soci_session_factory(), true);
  sql.begin();
  dal::Book r(sql);

  bl::Book unit_to_add;
  book_factory(unit_to_add);
  r.insert(unit_to_add);

  bl::Book unit_to_add2;
  book_factory_second(unit_to_add2);
  r.insert(unit_to_add2);
    
    bl::Book units[10];
    for (int i=0; i < 10; ++i){
        book_factory(units[i]);
        r.insert(units[i]);
    }
    
  std::list<bl::Book> list = r.base_functions_.rows();
  EXPECT_TRUE( unit_to_add == list.front());
  EXPECT_TRUE( units[9] == list.back());
  EXPECT_EQ( 12, list.size());
  std::vector<bl::Book> v = r.rows("", dal::Book::RECENTLY_ADDED);
  EXPECT_EQ( 12, v.size());
    
  dal::BookOnShelf bs(sql);
  boost::uuids::random_generator gen;
  bl::db_id b1( gen());
  bl::db_id s1( gen());
  
  std::pair<bl::db_id, bl::db_id> u1(unit_to_add,s1);
  bs.insert(u1);
    
  v = r.rows("", dal::Book::RECENTLY_ADDED, s1, false);
  EXPECT_EQ( 11, v.size());

  v = r.rows("", dal::Book::RECENTLY_ADDED, s1, true);
  EXPECT_EQ( 1, v.size());

    
  v = r.rows("aav", dal::Book::RECENTLY_ADDED, s1, false);
    EXPECT_EQ( 10, v.size());
    
  v = r.rows("aav", dal::Book::RECENTLY_ADDED, s1, true);
    EXPECT_EQ( 1, v.size());

  v = r.rows("GGGG", dal::Book::RECENTLY_ADDED, s1, false);
    EXPECT_EQ( 0, v.size());
    
  v = r.rows("GGGG", dal::Book::RECENTLY_ADDED, s1, true);
    EXPECT_EQ( 0, v.size());
  sql.rollback();

}
