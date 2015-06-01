#include "src/dal/base.hpp"
#include "src/dal/book.hpp"

#define CONNECTIN_NUM 10

namespace dal{

    soci::connection_pool& soci_session_factory() {
        static soci::connection_pool pool(CONNECTIN_NUM);
        return pool;
    }
    
    
std::string db_file_name() {
   std::string str ("../../db_files/tests.sqlite");
   return str;
}
   
    void init_db_pool() {
        const size_t poolSize = CONNECTIN_NUM;
        soci::connection_pool &pool = soci_session_factory();
        std::string data_base_file = db_file_name();
        for (size_t i = 0; i != poolSize; ++i)
        {
            soci::session & sql = pool.at(i);
            sql.open(soci::sqlite3, data_base_file);
        }
    }
    
    
    std::string get_resource_file_as_str(std::string f_name ){
        std::ifstream f_stream(std::string("../../../../../doc/") + f_name );
        std::stringstream buffer;
        buffer << f_stream.rdbuf();
        return buffer.str();
    }
}
