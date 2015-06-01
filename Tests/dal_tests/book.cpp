#include <gtest/gtest.h>

#include "src/details/utils.hpp"
#include "src/dal/Book.hpp"

void book_factory(bl::Book& unit_to_add){
    unit_to_add.title_ = "алиса";
    unit_to_add.authors_ = "aavЗО_%";
    unit_to_add.isbn_ = "drop table books";
    unit_to_add.cover_mime_type_ = "1925 ds";
    unit_to_add.file_name_ = "alisa.epub";
    unit_to_add.mime_type_ = "epub";
    
    dal::id::set(unit_to_add.book_instance_, boost::uuids::random_generator() ());
    dal::id::set(unit_to_add.book_cover_,  boost::uuids::random_generator() ());
    dal::id::set(unit_to_add.book_shelf_,  boost::uuids::random_generator() ());
    
    unit_to_add.last_read_ = pt::ptime(gr::date(2010,1,20), pt::hours(15));
    unit_to_add.install_time_ = pt::ptime(gr::date(2020,1,20), pt::hours(14));
    
    pt::ptime result_date = pt::ptime(gr::date(2000,1,20), pt::hours(12));
    unit_to_add.reading_time_ = pt::time_period(result_date, pt::hours(11));

}

void book_factory_second(bl::Book& unit_to_add){
    unit_to_add.title_ = "tiger dd";
    unit_to_add.authors_ = "ddd dsf";
    unit_to_add.isbn_ = "92830123";
    unit_to_add.cover_mime_type_ = "dddf dfsaf ";
    unit_to_add.file_name_ = " __ #sr.data";
    unit_to_add.mime_type_ = "pdf";
    dal::id::set(unit_to_add.book_instance_, boost::uuids::random_generator() ());
    dal::id::set(unit_to_add.book_cover_,  boost::uuids::random_generator() ());
    dal::id::set(unit_to_add.book_shelf_,  boost::uuids::random_generator() ());
    
    unit_to_add.last_read_ = pt::ptime(gr::date(2011,3,22), pt::hours(11));
    unit_to_add.install_time_ = pt::ptime(gr::date(2021,4,23), pt::hours(21));
    
    pt::ptime result_date = pt::ptime(gr::date(2002,1,20), pt::hours(2));
    unit_to_add.reading_time_ = pt::time_period(result_date, pt::hours(1));
    
}

TEST(sql_Book_, serialization_deserialization) {

  soci::session sql(dal::soci_session_factory());
  sql.begin();
  dal::Book r(sql);

  bl::Book unit_to_add;
  book_factory(unit_to_add);
    
  r.insert(unit_to_add);

  bl::Book
    from_data_base(r.base_functions_.get( dal::id::get(unit_to_add)));
  EXPECT_TRUE( unit_to_add == from_data_base);
  sql.rollback();
}

TEST(sql_Book_, Delete) {

  soci::session sql(dal::soci_session_factory());
  sql.begin();

  dal::Book r(sql);
  bl::Book unit_to_add;
  book_factory(unit_to_add);
  r.insert(unit_to_add);
  int rows = r.base_functions_.count();
  r.base_functions_.del( dal::id::get(unit_to_add));
  bl::Book from_data_base(
         r.base_functions_.get(dal::id::get(unit_to_add)));

  EXPECT_FALSE( from_data_base == unit_to_add);
  EXPECT_EQ( rows -1, r.base_functions_.count());
  sql.rollback();
}

TEST(sql_Book_, update) {

  soci::session sql(dal::soci_session_factory());
  sql.begin();
  dal::Book r(sql);

  bl::Book unit_to_add;
  book_factory(unit_to_add);
  r.insert(unit_to_add);
  bl::Book from_data_base(r.base_functions_.get(dal::id::get(unit_to_add)));
  
  EXPECT_TRUE( unit_to_add == from_data_base);
  book_factory_second(unit_to_add);
  
  r.update(unit_to_add);
  from_data_base = r.base_functions_.get(dal::id::get(unit_to_add));
  EXPECT_TRUE( unit_to_add == from_data_base);
  sql.rollback();
}

TEST(sql_Book_, get_list) {
    
  soci::session sql(dal::soci_session_factory());
  sql.begin();
  dal::Book r(sql);

  bl::Book unit_to_add;
  book_factory(unit_to_add);

  r.insert(unit_to_add);

  bl::Book unit_to_add2;
  book_factory_second(unit_to_add2);
  r.insert(unit_to_add2);
  std::list<bl::Book>   list = r.base_functions_.rows();
  EXPECT_TRUE( unit_to_add == list.front());
  EXPECT_TRUE( unit_to_add2 == list.back());
  EXPECT_EQ( 2, list.size());
  sql.rollback();
}
