
#ifndef details_utils_hpp
#define details_utils_hpp
#include "boost/date_time.hpp"
namespace gr = boost::gregorian;
namespace pt = boost::posix_time;
namespace utils {
   
  pt::time_duration total_seconds_to_time_duration(boost::int64_t value);
  boost::int64_t time_duration_to_total_seconds(pt::time_duration value);

  tm TO_TM(pt::ptime p);
  pt::ptime FROM_TM(const tm& t);

  pt::ptime infinit_to_fix(const pt::ptime& t);
  pt::ptime fix_to_infinit(const pt::ptime& t);

  boost::int64_t to_miliseconds_from_epoch(const pt::ptime& t);
  pt::ptime from_miliseconds_from_epoch(boost::int64_t t);
  
  
  pt::ptime convert(std::string t);
  std::string convert(const pt::ptime& t);
}
#endif
