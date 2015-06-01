#ifndef dal_Book_hpp
#define dal_Book_hpp

#include <string>
#include "src/bl/book.hpp"
#include "src/dal/base.hpp"
#include "src/dal/id.hpp"
#include "src/dal/book_on_shelf.hpp"


namespace dal {
  template<typename T> class base;
}

namespace soci
{
  template <>
  class type_conversion<bl::Book> {
   public:
    typedef values base_type;
    static void from_base(const values& v, indicator &in,
                          bl::Book& p) {

      p.id_ = dal::convert::id(v.get<dal::convert::type>("id"));
      p.title_ =  v.get<std::string>("title");
      p.authors_ =  v.get<std::string>("authors");
      p.isbn_ =  v.get<std::string>("isbn");
      p.cover_mime_type_ =  v.get<std::string>("cover_mime_type");
      p.file_name_ =  v.get<std::string>("file_name");
      p.mime_type_ =  v.get<std::string>("mime_type");
      
      p.book_instance_.id_ = dal::convert::id(v.get<dal::convert::type>("book_instance"));
      p.book_cover_.id_ = dal::convert::id(v.get<dal::convert::type>("book_cover"));
      p.book_shelf_.id_ = dal::convert::id(v.get<dal::convert::type>("book_shelf"));
     
      p.last_read_ = utils::FROM_TM(v.get<tm>("last_read"));
      p.install_time_ = utils::FROM_TM(v.get<tm>("install_time"));
        
      p.reading_time_ = pt::time_period(utils::FROM_TM(v.get<tm>("reading_begin")),
        utils::FROM_TM(v.get<tm>("reading_end")) );

    }
 
    static void to_base(const bl::Book& p,
                        values & v, indicator& ind) {
      v.set("id", dal::convert::id(p.id_));
      v.set("title", p.title_);
      v.set("authors", p.authors_);
      v.set("isbn", p.isbn_);
      v.set("cover_mime_type", p.cover_mime_type_);
      v.set("file_name", p.file_name_);
      v.set("mime_type", p.mime_type_);
     
      v.set("book_instance", dal::convert::id(p.book_instance_.id_));
      v.set("book_cover", dal::convert::id(p.book_cover_.id_));
      v.set("book_shelf", dal::convert::id(p.book_shelf_.id_));
        
      v.set("last_read", utils::TO_TM(p.last_read_));
      v.set("install_time", utils::TO_TM(p.install_time_));
        
      v.set("reading_begin", utils::TO_TM(p.reading_time_.begin()));
      v.set("reading_end", utils::TO_TM(p.reading_time_.end()));

      ind = i_ok;
    }
  };
}

namespace dal{

struct Book {
    
    enum SortOrder {
        TITLE,
        AUTHOR,
        RECENTLY_OPEN,
        RECENTLY_ADDED
    };
    
  Book(soci::session& sql) :
      base_functions_(sql, "books") {
  }
  void insert( bl::Book& r) {
    dal::id::set(r, boost::uuids::random_generator() ());
    base_functions_.sql() << "INSERT INTO "<< base_functions_.table_name() <<
      " (id,  title, authors, isbn, cover_mime_type, file_name, mime_type,"
      "  book_instance, book_cover, book_shelf, last_read, install_time,"
      "  reading_begin, reading_end)"
      " VALUES(:id, :title, :authors, :isbn, :cover_mime_type, :file_name, :mime_type,"
      "  :book_instance, :book_cover, :book_shelf, :last_read, :install_time,"
      "  :reading_begin,  :reading_end)",
            soci::use(r);
  }
  void update(const bl::Book& r) {
       const std::string& str_id = convert::id(dal::id::get(r));
    base_functions_.sql() << "UPDATE " << base_functions_.table_name() <<
        " SET"
        " title = :title,"
        " authors = :authors,"
        " isbn= :isbn,"
        " cover_mime_type= :cover_mime_type,"
        " file_name= :file_name,"
        " mime_type= :mime_type,"
        " book_instance = :book_instance,"
        " book_cover= :book_cover,"
        " book_shelf= :book_shelf,"
        " last_read= :last_read,"
        " install_time= :install_time,"
        " reading_begin= :reading_begin,"
        " reading_end= :reading_end"
        " WHERE id=:id",
          soci::use(str_id, "id"),soci::use(r);
  }

  std::string make_order_by(SortOrder sort_by){
      std::string order_by;
      switch(sort_by) {
          case TITLE:
              order_by = "title ASC";
              break;
          case AUTHOR:
              order_by = "authors ASC";
              break;
          case RECENTLY_ADDED:
              order_by = "install_time ASC";
              break;
          case RECENTLY_OPEN:
              order_by = "last_read DESC";
              break;
          default:
              order_by = "last_read DESC";
      }

      return order_by;
  }
    
  std::vector<bl::Book> rows(std::string search, SortOrder sort_by) {
        using namespace soci;
      
      std::string order_by = make_order_by(sort_by);
      
      std::stringstream query;
      query << "SELECT * FROM " << base_functions_.table_name();
      if(search.length() > 0){
          query << " WHERE authors LIKE '%" << search <<"%'";
          query << " OR title LIKE '%" << search <<"%'";
          query << " ORDER BY " << order_by;
          
      } else {
          query << " ORDER BY " << order_by;
      }
        rowset<bl::Book> rows = base_functions_.sql().prepare << query.str();
      

        std::vector<bl::Book> result;
        typename rowset<bl::Book>::const_iterator it;
        for (it = rows.begin(); it != rows.end(); ++it)  {
            result.push_back(*it);
        }
        
        return result;
    }

    std::vector<bl::Book> rows(std::string search,
                               SortOrder sort_by,
                               bl::db_id shelf,
                               bool belongs_to_shelf = true) {
        using namespace soci;
        
        std::string order_by = make_order_by(sort_by);
        
        std::stringstream query;
        query << "SELECT A.* FROM " << base_functions_.table_name() << " A";
        if(belongs_to_shelf) {
            query << " INNER JOIN " << dal::BookOnShelf::table_name() << " B";
        } else {
            query << " LEFT JOIN (SELECT * FROM "<< dal::BookOnShelf::table_name() <<
            " WHERE  shelf_id=:shelf_id) B";
        }

        query << " ON A.id=B.book_id ";
        
        if(search.length() > 0){
            query << " WHERE (A.authors LIKE '%" << search <<"%'";
            query << " OR A.title LIKE '%" << search <<"%')";
            if(belongs_to_shelf == false) {
                query << " AND B.book_id IS NULL ";
            } else {
                query << " AND B.shelf_id=:shelf_id";
            }

            query << " ORDER BY A." << order_by;
            
        } else {
            if(belongs_to_shelf == false) {
                query << " WHERE B.book_id IS NULL ";
            } else {
                query << " WHERE B.shelf_id=:shelf_id";
            }
            query << " ORDER BY A." << order_by;
        }
        const std::string& str_id = convert::id(dal::id::get(shelf));
        rowset<bl::Book> rows = (base_functions_.sql().prepare << query.str(),
                                    soci::use(str_id,"shelf_id"));
        
        std::vector<bl::Book> result;
        typename rowset<bl::Book>::const_iterator it;
        for (it = rows.begin(); it != rows.end(); ++it)  {
            result.push_back(*it);
        }
        
        return result;
    }
    
    
  Base<bl::Book> base_functions_;
  };

}
#endif
