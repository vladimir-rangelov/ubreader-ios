#ifndef commands_applicationcommandprocesso_hpp
#define commands_applicationcommandprocesso_hpp


#include <string>


namespace commands{

struct processor {

  template < class T >

  static typename T::result_type execute( T& cmd ) {

    return cmd.execute();

  }

};

}

#endif
