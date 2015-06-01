#include <thread>
#include <gtest/gtest.h>

#include "src/details/utils.hpp"
#include "src/dal/book_attributes.hpp"

#define TABLE_NAME "instances"

TEST(sql_exclusive_connection, two_threads_write_to_db) {
  
  auto f1 = []{
      bl::db_id book;
      
      bl::BookAttributes unit_to_add;
      
      std::string buf("ds cng dffdff de3");
      unit_to_add.raw_data_.reserve(buf.length());
      unit_to_add.raw_data_.assign(buf.begin(), buf.end());
      for (int i = 0; i < 100; ++i) {
          soci::session sql(dal::soci_session_factory(), true);
          sql.begin();
          dal::BookAttributes dba(sql, TABLE_NAME);
          
          dal::id::set(book, boost::uuids::random_generator()());
          unit_to_add.book_ = book;
          dba.insert(unit_to_add);
          //std::cout << "thread 1 insert:" << i << "\n";
          sql.rollback();
      }

  };
  auto f2 = []{
      bl::db_id book;

      bl::BookAttributes unit_to_add;
      
      std::string buf("ds cng dffdff de3");
      unit_to_add.raw_data_.reserve(buf.length());
      unit_to_add.raw_data_.assign(buf.begin(), buf.end());
        for (int i = 0; i < 100; ++i) {
            soci::session sql(dal::soci_session_factory(), true);
            sql.begin();
            dal::BookAttributes dba(sql, TABLE_NAME);
            dal::id::set(book, boost::uuids::random_generator()());
            unit_to_add.book_ = book;
            dba.insert(unit_to_add);
           // std::cout << "thread 2 insert:" << i << "\n";
            sql.rollback();
        }

  };

    std::thread t1(f1);
    std::thread t2(f2);
    t1.join();
    t2.join();
}


TEST(sql_exclusive_connection, two_threads_read_from_db) {
    
    soci::session sql(dal::soci_session_factory());
    sql.begin();
    dal::BookAttributes dba(sql, TABLE_NAME);
    bl::BookAttributes unit_to_add;

    std::string buf("ds cng dffdff de3");
    unit_to_add.raw_data_.reserve(buf.length());
    unit_to_add.raw_data_.assign(buf.begin(), buf.end());
    
    bl::db_id book;
    dal::id::set(book, boost::uuids::random_generator()());
    unit_to_add.book_ = book;
    dba.insert(unit_to_add);

    
    
    auto f1 = [&book] {
        for (int i = 0; i < 500; ++i) {
            soci::session sql(dal::soci_session_factory());
            dal::BookAttributes dba(sql, TABLE_NAME);
            bl::BookAttributes unit_from_db;
            dba.get(unit_from_db, book);
            //std::cout << "thread 1 reading: " << i << "\n";
        }
        
    };
    auto f2 = [&book]{

        for (int i = 0; i < 500; ++i) {
            soci::session sql(dal::soci_session_factory());
            dal::BookAttributes dba(sql, TABLE_NAME);
            bl::BookAttributes unit_from_db;
            dba.get(unit_from_db, book);
            //std::cout << "thread 2 reading: " << i << "\n";
        }
        
    };
    sql.rollback();
    std::thread t1(f1);
    std::thread t2(f2);
    t1.join();
    t2.join();
}

TEST(sql_exclusive_connection, two_thread_reads_two_thread_writes) {

    bl::db_id book;
    dal::id::set(book, boost::uuids::random_generator()());
    
    {
        soci::session sql(dal::soci_session_factory());
        dal::BookAttributes dba(sql, TABLE_NAME);
        bl::BookAttributes unit_to_add;
        
        std::string buf("ds cng dffdff de3");
        unit_to_add.raw_data_.reserve(buf.length());
        unit_to_add.raw_data_.assign(buf.begin(), buf.end());

        unit_to_add.book_ = book;
        dba.insert(unit_to_add);
    }
    
    auto f1 = [&book] {
        for (int i = 0; i < 100; ++i) {
            soci::session sql(dal::soci_session_factory());
            dal::BookAttributes dba(sql, TABLE_NAME);
            bl::BookAttributes unit_from_db;
            dba.get(unit_from_db, book);
           // std::cout << "thread 1 reading: " << i << "\n";
        }
        
    };
    auto f2 = [&book]{
        
        for (int i = 0; i < 100; ++i) {
            soci::session sql(dal::soci_session_factory(), true);
            dal::BookAttributes dba(sql, TABLE_NAME);
            bl::BookAttributes unit_from_db;
            dba.get(unit_from_db, book);
           // std::cout << "thread 2 writing: " << i << "\n";
        }
        
    };

    auto f3 = [&book]{
        
        for (int i = 0; i < 100; ++i) {
            soci::session sql(dal::soci_session_factory());
            dal::BookAttributes dba(sql, TABLE_NAME);
            bl::BookAttributes unit_from_db;
            dba.get(unit_from_db, book);
            //std::cout << "thread 3 reading: " << i << "\n";
        }
        
    };
    
    
    auto f4 = [&book]{
        
        for (int i = 0; i < 100; ++i) {
            soci::session sql(dal::soci_session_factory(), true);
            dal::BookAttributes dba(sql, TABLE_NAME);
            bl::BookAttributes unit_from_db;
            dba.get(unit_from_db, book);
           // std::cout << "thread 4 writing: " << i << "\n";
        }
        
    };
    
    {
        soci::session sql(dal::soci_session_factory());
        dal::BookAttributes dba(sql, TABLE_NAME);
        dba.del(book.id());
    }
    
    std::thread t1(f1);
    std::thread t2(f2);
    std::thread t3(f3);
    std::thread t4(f4);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
