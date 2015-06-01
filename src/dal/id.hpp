#ifndef db_id_manipulator_hpp
#define db_id_manipulator_hpp

#include "src/bl/db_id.hpp"

namespace soci{
template <>
class type_conversion<bl::db_id> {
public:
    typedef values base_type;
    static void from_base(const values& v, indicator &in,
                          bl::db_id& p) {
        
        p.id_ = dal::convert::id(v.get<dal::convert::type>("id"));
    }
    
    
    static void to_base(const bl::db_id& p,
                        values & v, indicator& ind) {
        v.set("id", dal::convert::id(p.id_));
        ind = i_ok;
    }
};
}

namespace dal {

struct id{
  static const boost::uuids::uuid& get(const bl::db_id& i) {
    return i.id_;
  }
  static void set(bl::db_id& i,
                                 const boost::uuids::uuid& id) {
    i.id_= id;
  }

};


}
#endif
