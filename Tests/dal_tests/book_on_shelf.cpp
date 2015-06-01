#include <gtest/gtest.h>

#include "src/details/utils.hpp"
#include "src/dal/Book.hpp"
#include "src/dal/book_on_shelf.hpp"



TEST(sql_Book_on_shelf, all_base_operations) {
    soci::session sql(dal::soci_session_factory(), true);
    sql.begin();
    dal::BookOnShelf r(sql);
    
    boost::uuids::random_generator gen;
    bl::db_id b1( gen());
    bl::db_id b2( gen());
    bl::db_id b3( gen());
    bl::db_id s1( gen());
    bl::db_id s2( gen());
    bl::db_id s3( gen());

    std::pair<bl::db_id, bl::db_id> u1(b1,s1);
    std::pair<bl::db_id, bl::db_id> u2(b2,s2);
    std::pair<bl::db_id, bl::db_id> u3(b2,s3);
    std::pair<bl::db_id, bl::db_id> u4(b3,s2);
    
    EXPECT_EQ(0,r.count());
    r.insert(u1);
    r.insert(u2);
    r.insert(u3);
    r.insert(u4);
    EXPECT_EQ(4,r.count());
    
    std::list<std::pair<bl::db_id, bl::db_id> > result;
    result = r.rows_shelfs_for_book(b1.id());
    EXPECT_EQ(1,result.size());
    std::pair<bl::db_id, bl::db_id>
    from_data_base(result.front());
    EXPECT_TRUE( u1 == from_data_base);
    result = r.rows_book_on_shelf( s1.id());
    EXPECT_EQ(1,result.size());
    from_data_base = result.front();
    EXPECT_TRUE( u1 == from_data_base);
    
    EXPECT_EQ(1,r.count_shelf(s1.id()));
    EXPECT_EQ(1,r.count_books(b1.id()));
    EXPECT_EQ(1,r.count_books(b3.id()));
    EXPECT_EQ(2,r.count_shelf(s2.id()));
    EXPECT_EQ(2,r.count_books(b2.id()));
    r.del_book_records(b2.id());
    EXPECT_EQ(0,r.count_books(b2.id()));
    EXPECT_EQ(1,r.count_shelf(s2.id()));
    r.del_shelf_records(s2.id());
    EXPECT_EQ(0,r.count_shelf(s2.id()));

    EXPECT_EQ(1,r.count());
    r.delete_all_rows();
    EXPECT_EQ(0,r.count());
    
    
    
    try
    {
        r.insert(u1);
        r.insert(u1);//must throw
        EXPECT_TRUE(false);
    }
    catch (...)
    {
    }
    

    
    sql.rollback();
}

