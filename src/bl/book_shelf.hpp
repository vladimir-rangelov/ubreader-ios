#ifndef bl_book_shelf_hpp
#define bl_book_shelf_hpp

#include "src/bl/db_id.hpp"

namespace bl {

class BookShelf : public db_id {
 
public:
  BookShelf() : sort_order_(0){ }
  std::string name_;
  db_id parent_;
  int sort_order_;
  bool operator==(const BookShelf& right){
    return db_id::operator==(right) &&
       name_==right.name_ &&
       sort_order_ == right.sort_order_ &&
	   parent_==right.parent_;
  }
};

}
#endif
