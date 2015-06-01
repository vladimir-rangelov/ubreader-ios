#ifndef bl_book_attributes_hpp
#define bl_book_attributes_hpp

#include <vector>
#include "src/bl/db_id.hpp"

namespace bl {

class BookAttributes {
 
public:
  BookAttributes() { }
  db_id book_;
  std::vector<char> raw_data_;
  
  bool operator==(const BookAttributes& right){
    return 
       book_==right.book_ &&
	   raw_data_==right.raw_data_;
  }
  void resize_raw_data(std::size_t s){
      raw_data_.resize(s);
  }
  char* raw_ptr(){
    return raw_data_.data();
  }
};

}
#endif
