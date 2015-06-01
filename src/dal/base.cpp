
#include <boost/algorithm/string.hpp>

#include "src/commands/import_new_books.hpp"
#include "src/dal/base.hpp"
#include "src/dal/book.hpp"

namespace dal{

    void drop_database(soci::session& sql ){
        std::string delete_tables_sql = get_resource_file_as_str("drop_tables.txt");
        std::vector<std::string> res;
        boost::split(res, delete_tables_sql, boost::is_any_of(";"));
        try {
        std::for_each(res.begin(), res.end(),
                      [&sql] ( std::string val)
                      {
                          sql << val<<";";
                      });
        } catch (...) {

        }

    }
    
    void create_all_tables(soci::session& sql ){
        std::string create_tables_sql = get_resource_file_as_str("sql_tables.txt");
        std::vector<std::string> res;
        boost::split(res, create_tables_sql, boost::is_any_of(";"));
        try {
        std::for_each(res.begin(), res.end(),
                      [&sql] ( std::string val)
                      {
                          sql << val<<";";
                      });
        } catch (...) {
            
        }
    }
    
    bool books_table_exist(soci::session& sql ){
        std::string query ="SELECT count(*) FROM sqlite_master WHERE name='";
        int result = 0;
        dal::Book b(sql);
        sql << query << b.base_functions_.table_name()<<"'", soci::into(result);
        return result !=0;
    }
}
