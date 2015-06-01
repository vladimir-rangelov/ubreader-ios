#ifndef dal_base_hpp
#define dal_base_hpp

#include <string>
#include <list>
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "soci.h"
#include <soci-sqlite3.h>
#include "src/details/utils.hpp"
#include "src/bl/db_id.hpp"

#define soci_connection_pool_size 10

namespace dal{

struct convert {
  typedef std::string type;
  static type id(boost::uuids::uuid id) {
    std::stringstream id_stream;
    id_stream << id;
    return id_stream.str();
  }
  static type id(bl::db_id dbId) {
        return id(dbId.id_);
  }
  static boost::uuids::uuid id(type id) {
    boost::uuids::uuid result;
    std::stringstream str_id(id);
    str_id >> result;
    return result;
  }
};


template<typename T>
class Base {
 public:
  explicit Base(soci::session& sql, std::string table_name) :
      sql_(sql), table_name_(table_name) { }

  std::list<T> rows() {
    using namespace soci;

    rowset<T> rows = sql_.prepare << "SELECT * FROM " << table_name_;
    std::list<T> result;
    typename rowset<T>::const_iterator it;
    for (it = rows.begin(); it != rows.end(); ++it)  {
      result.push_back(*it);
    }

    return result;
  }

  void delete_all_rows() {
    sql_ << "DELETE  FROM " << table_name_;
  }

  T get(boost::uuids::uuid id) {
    T obj;
    const std::string& str_id = convert::id(id);
    sql_ << "SELECT * FROM "
         << table_name_
         << " WHERE id=:id  LIMIT 1",
           soci::into(obj), soci::use(str_id, "id");
    return obj;
  }
  void del(boost::uuids::uuid id) {
    const std::string& str_id = convert::id(id);
    sql_ << "DELETE FROM "<< table_name_<<" WHERE id=:id",
    soci::use(str_id, "id");
  }
    int count() {
        int c = 0;
        sql_ << "SELECT count(*) FROM " << table_name_,
        soci::into(c);
        return c;
    }
  soci::session& sql() { return sql_; }
  std::string table_name() { return table_name_; }
 private:
  soci::session& sql_;
  std::string table_name_;
};

void init_db_pool();
soci::connection_pool& soci_session_factory();

std::string get_resource_file_as_str(std::string f_name );
void drop_database(soci::session& sql);
void create_all_tables(soci::session& sql);
bool books_table_exist(soci::session& sql);

}
#endif