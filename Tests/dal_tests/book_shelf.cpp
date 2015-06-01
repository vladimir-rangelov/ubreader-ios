#include <gtest/gtest.h>

#include "src/details/utils.hpp"
#include "src/dal/book_shelf.hpp"

void book_shelf_factory(bl::BookShelf& unit_to_add){
    unit_to_add.name_ = "shelf 1";
    dal::id::set(unit_to_add.parent_, boost::uuids::random_generator() ());
}

void book_second_shelf_factory(bl::BookShelf& unit_to_add){
    unit_to_add.name_ = "shelf 2";
    dal::id::set(unit_to_add.parent_, boost::uuids::random_generator() ());
}

TEST(sql_BookShelf, serialization_deserialization) {

  soci::session sql(dal::soci_session_factory());
  sql.begin();
  dal::BookShelf r(sql);

  bl::BookShelf unit_to_add;
  book_shelf_factory(unit_to_add);
    
  r.insert(unit_to_add);

  bl::BookShelf
    from_data_base(r.base_functions_.get( dal::id::get(unit_to_add)));
  EXPECT_TRUE( unit_to_add == from_data_base);
    sql.rollback();

}

TEST(sql_BookShelf, Delete) {

  soci::session sql(dal::soci_session_factory());
  sql.begin();

  dal::BookShelf r(sql);
  bl::BookShelf unit_to_add;
  book_shelf_factory(unit_to_add);
  r.insert(unit_to_add);
  int rows = r.base_functions_.count();
  r.base_functions_.del( dal::id::get(unit_to_add));
  bl::BookShelf from_data_base(
         r.base_functions_.get(dal::id::get(unit_to_add)));

  EXPECT_FALSE( from_data_base == unit_to_add);
  EXPECT_EQ( rows -1, r.base_functions_.count());
    sql.rollback();

}

TEST(sql_BookShelf, update) {

  soci::session sql(dal::soci_session_factory());
  sql.begin();
  dal::BookShelf r(sql);

  bl::BookShelf unit_to_add;
  book_shelf_factory(unit_to_add);
  r.insert(unit_to_add);
  bl::BookShelf from_data_base(r.base_functions_.get(dal::id::get(unit_to_add)));
  
  EXPECT_TRUE( unit_to_add == from_data_base);
  book_second_shelf_factory(unit_to_add);
  
  r.update(unit_to_add);
  from_data_base = r.base_functions_.get(dal::id::get(unit_to_add));
  EXPECT_TRUE( unit_to_add == from_data_base);
    sql.rollback();

}

TEST(sql_BookShelf, get_list) {

  soci::session sql(dal::soci_session_factory());
  sql.begin();
  dal::BookShelf r(sql);

  bl::BookShelf unit_to_add;
  book_shelf_factory(unit_to_add);

  r.insert(unit_to_add);

  bl::BookShelf unit_to_add2;
  book_second_shelf_factory(unit_to_add2);
  r.insert(unit_to_add2);
  std::list<bl::BookShelf>   list = r.base_functions_.rows();
  EXPECT_TRUE( unit_to_add == list.front());
  EXPECT_TRUE( unit_to_add2 == list.back());
  EXPECT_EQ( 2, list.size());
    sql.rollback();

}
