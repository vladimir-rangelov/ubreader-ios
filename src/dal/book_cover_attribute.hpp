#ifndef dal_BookCoverAttribute_hpp
#define dal_BookCoverAttribute_hpp

#include "src/dal/book_attributes.hpp"


namespace dal{
 struct BookCoverAttribute : public BookAttributes {
     BookCoverAttribute(soci::session& sql) : BookAttributes(sql, "covers") { }
 
  };
  struct BookCoverCacheAttribute : public BookAttributes {
        BookCoverCacheAttribute(soci::session& sql) : BookAttributes(sql, "covers_cache") { }
        
  };
}
#endif
