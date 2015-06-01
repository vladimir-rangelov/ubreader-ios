#include <gtest/gtest.h>

#include "src/details/utils.hpp"
#include "src/dal/base.hpp"

TEST(utils_fixture, base_period){
   pt::ptime pt_neg_infin = pt::ptime(pt::neg_infin);
   pt::ptime pt_pos_infin = pt::ptime(pt::pos_infin);
   tm t_neg_infin = utils::TO_TM(pt::ptime(pt::neg_infin));
   tm t_pos_infin = utils::TO_TM(pt::ptime(pt::pos_infin));

   EXPECT_TRUE( pt_neg_infin == utils::FROM_TM(t_neg_infin) );
   EXPECT_TRUE( pt_pos_infin == utils::FROM_TM(t_pos_infin) );

}
TEST(utils_fixture, total_seconds__time_duration1){

   pt::time_duration pt_neg_infin = pt::time_duration(pt::neg_infin);
   pt::time_duration pt_pos_infin = pt::time_duration(pt::pos_infin);
   boost::int64_t t_neg_infin = utils::time_duration_to_total_seconds(pt::time_duration(pt::neg_infin));
   boost::int64_t t_pos_infin = utils::time_duration_to_total_seconds(pt::time_duration(pt::pos_infin));

   EXPECT_TRUE( pt_neg_infin == utils::total_seconds_to_time_duration(t_neg_infin) );
   EXPECT_TRUE( pt_pos_infin == utils::total_seconds_to_time_duration(t_pos_infin) );
   

}

TEST(utils_fixture, to_epoch_rep__from_epoch_rep){

   pt::ptime time_to_serialize(gr::date(2000,1,1), pt::time_duration(1,2,3,0));
   boost::int64_t t_int = utils::to_miliseconds_from_epoch(time_to_serialize);
   EXPECT_EQ( time_to_serialize , utils::from_miliseconds_from_epoch(t_int) );
  
}


TEST(utils_fixture, infin_ptime){

   pt::ptime pt_neg_infin = pt::ptime(pt::neg_infin);
   pt::ptime pt_pos_infin = pt::ptime(pt::pos_infin);
   boost::int64_t n_infin = utils::to_miliseconds_from_epoch(pt_neg_infin);
   pt::ptime pt_neg_infin2 = utils::from_miliseconds_from_epoch(n_infin);

   EXPECT_EQ( pt_neg_infin2 , pt::ptime(pt::neg_infin) );
  
   boost::int64_t p_infin = utils::to_miliseconds_from_epoch(pt_pos_infin);
   pt::ptime pt_pos_infin2 = utils::from_miliseconds_from_epoch(p_infin);

   EXPECT_EQ( pt_pos_infin , pt::ptime(pt::pos_infin) );
  
}

TEST(utils_fixture, nil_guid_test_room_type) {
  boost::uuids::uuid nil_id =
    boost::uuids::string_generator()("00000000-0000-0000-0000-000000000000");
  EXPECT_TRUE( nil_id.is_nil());
}

TEST(utils_fixture, time_from_iso_to_iso) {

 std::string begin_iso_data ("20100426");

 const pt::ptime begine_from_date (gr::date(2010,4,26));
 const pt::ptime begine_from_iso(utils::convert(begin_iso_data));

 EXPECT_EQ(begine_from_date , begine_from_iso);
}

TEST(utils_fixture, period_end) {

 pt::ptime begin(gr::date(2000,1,1));
 pt::ptime end(gr::date(2000,1,2));
 pt::time_period period( begin, end);

 EXPECT_EQ(end , period.end());
}

TEST(utils_fixture, sqlite3_threadsafe) {

  EXPECT_TRUE(0 != sqlite_api::sqlite3_threadsafe());
}
