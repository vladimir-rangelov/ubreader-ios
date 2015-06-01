#ifndef dal_book_on_shelf_hpp
#define dal_book_on_shelf_hpp

#include <utility>
#include "src/dal/base.hpp"

namespace soci{
    template <>
    class type_conversion< std::pair< bl::db_id, bl::db_id> > {
    public:
        typedef values base_type;
        static void from_base(const values& v, indicator &in,
                              std::pair< bl::db_id, bl::db_id>& p) {
            
            p.first.id_ = dal::convert::id(v.get<dal::convert::type>("book_id"));
            p.second.id_ = dal::convert::id(v.get<dal::convert::type>("shelf_id"));

        }
        
        static void to_base(const std::pair< bl::db_id, bl::db_id>& p,
                            values & v, indicator& ind) {
            v.set("book_id", dal::convert::id(p.first.id_));
            v.set("shelf_id", dal::convert::id(p.second.id_));
            ind = i_ok;
        }
    };
}


namespace dal{

class BookOnShelf {
 public:
    explicit BookOnShelf(soci::session& sql) :
      sql_(sql) { }


    void delete_all_rows() {
        sql_ << "DELETE  FROM " << table_name();
    }

    void insert( std::pair< bl::db_id, bl::db_id>& r) {
        sql_ << "INSERT INTO "<< table_name() <<
        " (book_id,  shelf_id)"
        " VALUES(:book_id, :shelf_id)",
        soci::use(r);
    }
    
    
    std::list<std::pair< bl::db_id, bl::db_id> >
        rows_shelfs_for_book(boost::uuids::uuid book_id) {
        using namespace soci;
        const std::string& str_id = convert::id(book_id);
        rowset<std::pair< bl::db_id, bl::db_id> > rows =
            (sql_.prepare << "SELECT * FROM " << table_name() <<" WHERE book_id=:id",
             soci::use(str_id, "id"));
        std::list<std::pair< bl::db_id, bl::db_id> > result;
        typename rowset<std::pair< bl::db_id, bl::db_id> >::const_iterator it;
        for (it = rows.begin(); it != rows.end(); ++it)  {
            result.push_back(*it);
        }
        return result;
    }
    
    std::list<std::pair< bl::db_id, bl::db_id> >
        rows_book_on_shelf(boost::uuids::uuid shelf_id) {
        using namespace soci;
        const std::string& str_id = convert::id(shelf_id);
        rowset<std::pair< bl::db_id, bl::db_id> > rows = (sql_.prepare << "SELECT * FROM " << table_name() <<" WHERE shelf_id=:id",
        soci::use(str_id, "id"));
        std::list<std::pair< bl::db_id, bl::db_id> > result;
        typename rowset<std::pair< bl::db_id, bl::db_id>>::const_iterator it;
        for (it = rows.begin(); it != rows.end(); ++it)  {
            result.push_back(*it);
        }
        return result;
    }
    
    void del_book_records(boost::uuids::uuid book_id) {
        const std::string& str_id = convert::id(book_id);
        sql_ << "DELETE FROM "<< table_name()<<" WHERE book_id=:id",
        soci::use(str_id, "id");
    }
    
    void del_shelf_records(boost::uuids::uuid shelf_id) {
        const std::string& str_id = convert::id(shelf_id);
        sql_ << "DELETE FROM "<< table_name()<<" WHERE shelf_id=:id",
        soci::use(str_id, "id");
    }
    
    int count_books(boost::uuids::uuid book_id) {
        int c = 0;
        const std::string& str_id = convert::id(book_id);
        sql_ << "SELECT count(*) FROM " << table_name() <<" WHERE book_id=:id",
        soci::into(c), soci::use(str_id, "id") ;
        return c;
    }

    int count_shelf(boost::uuids::uuid shelf_id) {
        int c = 0;
        const std::string& str_id = convert::id(shelf_id);
        sql_ << "SELECT count(*) FROM " << table_name() <<" WHERE shelf_id=:id",
        soci::into(c), soci::use(str_id, "id");
        return c;
    }
    
    int count() {

        int c = 0;
        sql_ << "SELECT count(*) FROM " << table_name(),
        soci::into(c);
        return c;
    }

    static std::string table_name(){
        return "book_on_shelf";
    }
 private:
  soci::session& sql_;

};

}
#endif