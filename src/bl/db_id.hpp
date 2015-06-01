#ifndef bussiness_layer_db_id_hpp
#define bussiness_layer_db_id_hpp

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"


namespace soci {
  template<class, typename Enable> class type_conversion;
  template<class> class rowset;

  namespace details {
   template<class> class rowset_impl;
  }
}

namespace dal {
  template<typename T> class base;
  template<typename> class code_name;
  class ControlledUnit;
  struct id;
  struct convert;
}

namespace bl {


struct db_id {
  db_id() : id_(boost::uuids::nil_generator() ()) { }
  db_id(const boost::uuids::uuid& id) : id_(id){ }

  bool operator==(const db_id& right) const {
    return (id_ == right.id_);
  }
  boost::uuids::uuid id()const{
      return id_;
  };
 private:
 
  friend struct dal::id;
  friend struct dal::convert;
  // dal layer
  template<typename, typename Enable> friend class soci::type_conversion;
  template<typename> friend class soci::rowset;
  template<typename> friend class soci::details::rowset_impl;
  template<typename> friend  class dal::base;

  boost::uuids::uuid id_;
};


}
#endif
