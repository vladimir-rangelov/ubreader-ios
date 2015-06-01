#ifndef bl_book_hpp
#define bl_book_hpp

#include "src/details/utils.hpp"
#include "src/bl/db_id.hpp"


namespace bl {

class Book : public db_id {
 
public:
  Book() : last_read_(pt::neg_infin), install_time_(pt::neg_infin),
           reading_time_(pt::ptime(pt::pos_infin), pt::ptime(pt::neg_infin) )
{ }
  std::string title_;
  std::string authors_;
  std::string isbn_;
  std::string cover_mime_type_;
  std::string file_name_;
  std::string mime_type_;
    

  db_id book_instance_;
  db_id book_cover_;
  db_id book_shelf_;
    
  pt::ptime install_time_;
  pt::ptime last_read_;
  pt::time_period reading_time_;
  
  bool operator==(const Book& right){
    return db_id::operator==(right) &&
       title_==right.title_ &&
	   authors_==right.authors_ &&
	   isbn_==right.isbn_ &&
       cover_mime_type_==right.cover_mime_type_ &&
       file_name_==right.file_name_ &&
       mime_type_==right.mime_type_ &&
       book_instance_==right.book_instance_ &&
       book_cover_==right.book_cover_ &&
       book_shelf_==right.book_shelf_ &&
       install_time_==right.install_time_ &&
       last_read_==right.last_read_ &&
	   reading_time_==right.reading_time_ ;
  }
};

}
#endif
