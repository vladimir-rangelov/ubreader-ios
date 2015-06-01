#include "src/details/utils.hpp"


namespace utils{

static const  boost::int64_t DURATION_POS_INFIN = 271389744000;
static const  boost::int64_t DURATION_NEG_INFIN =-271389744000;


pt::time_duration total_seconds_to_time_duration(boost::int64_t value) {
  
  if(value == DURATION_POS_INFIN){
    return pt::time_duration(pt::pos_infin);
  } else if(value == DURATION_NEG_INFIN){
    return pt::time_duration(pt::neg_infin);
  } 
  
  
  return  pt::time_duration( value / 3600 ,
                               (value % 3600)/ 60 ,
                               value %60,
                               0);
}
boost::int64_t time_duration_to_total_seconds(pt::time_duration value) {
  
  if(value == pt::time_duration(pt::pos_infin)){
    return DURATION_POS_INFIN;
  } else if(value == pt::time_duration(pt::neg_infin)){
    return DURATION_NEG_INFIN;
  } 
   
  return value.ticks()/value.ticks_per_second();
}


static const pt::ptime psevdo_pos_inf(gr::date(9999,12,31), pt::time_duration(0,0,0));
static const pt::ptime psevdo_neg_inf(gr::date(1900,1,1), pt::time_duration(0,0,0));
static const pt::ptime epoch(gr::date(1970,1,1), pt::time_duration(0,0,0));


pt::ptime infinit_to_fix(const pt::ptime& p){
  if(p < psevdo_neg_inf) {
    return psevdo_neg_inf;
  } else  if(p > psevdo_pos_inf) {
    return psevdo_pos_inf;
  }
  return p;
}

pt::ptime fix_to_infinit(const pt::ptime& t){
  if(t >= psevdo_pos_inf) {
    return pt::ptime(pt::pos_infin);
  } else  if(t <= psevdo_neg_inf) {
    return pt::ptime(pt::neg_infin);
  }
  return t;
}

tm TO_TM(pt::ptime p){
  return pt::to_tm(infinit_to_fix(p));
}

pt::ptime FROM_TM(const tm& t){
  pt::ptime result = pt::ptime_from_tm(t);
  return fix_to_infinit(result);
}

boost::int64_t to_miliseconds_from_epoch(const pt::ptime& t) {
  
  if(t == pt::ptime(pt::pos_infin)){
    return DURATION_POS_INFIN * 1000;
  } else if(t == pt::ptime(pt::neg_infin)){
    return DURATION_NEG_INFIN * 1000;
  } 
   
  pt::time_duration duration = t - epoch;
  boost::int64_t result = time_duration_to_total_seconds(duration);
  
  return result * 1000;
  
}

pt::ptime from_miliseconds_from_epoch(boost::int64_t t){
  t /=1000;
  if(t == DURATION_POS_INFIN){
    return pt::ptime(pt::pos_infin);
  } else if(t == DURATION_NEG_INFIN){
    return pt::ptime(pt::neg_infin);
  } 
  
  pt::ptime result = epoch + total_seconds_to_time_duration(t) ;
  return fix_to_infinit(result);
  
}

pt::ptime convert(std::string t){
if(t.find('T') == std::string::npos) {
  t += "T000000";
}
  return pt::from_iso_string(t);
}
std::string convert(const pt::ptime& t){
  return pt::to_iso_string(t);
  
}

}
