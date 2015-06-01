#ifndef dal_BookShelf_hpp
#define dal_BookShelf_hpp

#include <string>
#include "src/bl/book_shelf.hpp"
#include "src/dal/base.hpp"
#include "src/dal/id.hpp"

namespace dal {
  template<typename T> class base;
}

namespace soci
{
    template <>
    class type_conversion<bl::BookShelf> {
    public:
        typedef values base_type;
        static void from_base(const values& v, indicator &in,
                              bl::BookShelf& p) {
            
            p.id_ = dal::convert::id(v.get<dal::convert::type>("id"));
            p.parent_.id_ = dal::convert::id(v.get<dal::convert::type>("parent"));
            p.name_ =  v.get<std::string>("name");
            p.sort_order_ =  v.get<int>("sort_order");
        }
        
        static void to_base(const bl::BookShelf& p,
                            values & v, indicator& ind) {
            v.set("id", dal::convert::id(p.id_));
            v.set("parent", dal::convert::id(p.parent_.id_));
            v.set("name", p.name_);
            v.set("sort_order", p.sort_order_);
            ind = i_ok;
        }
    };
}

namespace dal{

struct BookShelf {
    BookShelf(soci::session& sql) :
      base_functions_(sql, "book_shelves") {
  }
  
    void insert( bl::BookShelf& r) {
        dal::id::set(r, boost::uuids::random_generator() ());
        base_functions_.sql() << "INSERT INTO "<< base_functions_.table_name() <<
        " (id, parent, name, sort_order )"
        " VALUES(:id, :parent, :name, :sort_order)",
        soci::use(r);
    }
    
    void update(const bl::BookShelf& r) {
        const std::string& str_id = convert::id(dal::id::get(r));
        base_functions_.sql() << "UPDATE " << base_functions_.table_name() <<
        " SET"
        " parent = :parent,"
        " name = :name,"
        " sort_order = :sort_order"
        " WHERE id=:id",
        soci::use(r),soci::use(str_id, "id");
    }
    
    std::vector<bl::BookShelf> sorted_rows() {
        using namespace soci;
        
        rowset<bl::BookShelf> rows = base_functions_.sql().prepare << "SELECT * FROM "
                                                  << base_functions_.table_name()
                                                  << " ORDER BY sort_order ASC";
        std::vector<bl::BookShelf> result;
        typename rowset<bl::BookShelf>::const_iterator it;
        for (it = rows.begin(); it != rows.end(); ++it)  {
            result.push_back(*it);
        }
        
        return result;
    }
    
    Base<bl::BookShelf> base_functions_;
  
    };

}
#endif
