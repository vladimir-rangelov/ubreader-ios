#include <gtest/gtest.h>

#include "src/dal/base.hpp"

TEST(sql_database_management, create_delete_check_database) {

  soci::session sql(dal::soci_session_factory());
  sql.begin();
  EXPECT_TRUE(dal::books_table_exist(sql));
    if(dal::books_table_exist(sql)){
        dal::drop_database(sql);
    }
  EXPECT_FALSE(dal::books_table_exist(sql));
  dal::create_all_tables(sql);
  EXPECT_TRUE(dal::books_table_exist(sql));
  sql.commit();
}



