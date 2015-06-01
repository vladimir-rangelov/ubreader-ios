#include <gtest/gtest.h>

#include "src/details/utils.hpp"
#include "src/dal/book_attributes.hpp"

#define TABLE_NAME "instances"

TEST(sql_BookAttributes, serialization_deserialization) {

 soci::session sql(dal::soci_session_factory());
  sql.begin();
  dal::BookAttributes dba(sql, TABLE_NAME);
  bl::db_id book;
  dal::id::set(book, boost::uuids::random_generator()());
    
  bl::BookAttributes unit_to_add;
  unit_to_add.book_ = book;

  std::string buf("ds cng dffdff de3");
  unit_to_add.raw_data_.reserve(buf.length());
  unit_to_add.raw_data_.assign(buf.begin(), buf.end());
  dba.insert(unit_to_add);    

  bl::BookAttributes from_data_base;
  dba.get(from_data_base, book );
    from_data_base.book_ = book;
  EXPECT_TRUE( unit_to_add == from_data_base);
    sql.rollback();

}

TEST(sql_BookAttributes, Delete) {

  soci::session sql(dal::soci_session_factory());
  sql.begin();

  dal::BookAttributes dba(sql, TABLE_NAME);
  bl::db_id book;
  dal::id::set(book, boost::uuids::random_generator()());

  bl::BookAttributes unit_to_add;
  unit_to_add.book_ = book;
  dba.insert(unit_to_add);
    
  int rows = dba.count();
  dba.del( dal::id::get(book));
  EXPECT_EQ( rows -1, dba.count());
    sql.rollback();

}

TEST(sql_BookAttributes, update) {
    
    soci::session sql(dal::soci_session_factory());
    sql.begin();
    dal::BookAttributes dba(sql, TABLE_NAME);
    bl::db_id book;
    dal::id::set(book, boost::uuids::random_generator()());
    
    bl::BookAttributes unit_to_add;
    unit_to_add.book_ = book;
    
    std::string buf("str1");
    unit_to_add.raw_data_.reserve(buf.length());
    unit_to_add.raw_data_.assign(buf.begin(), buf.end());
    dba.insert(unit_to_add);
    
    buf = "str1 dfsdf";
    unit_to_add.raw_data_.assign(buf.begin(), buf.end());
    dba.update(unit_to_add);
   
    bl::BookAttributes from_data_base;
    dba.get(from_data_base, book );
    from_data_base.book_ = book;
    EXPECT_TRUE( unit_to_add == from_data_base);
    sql.rollback();
}

