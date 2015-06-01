#ifndef commands_DeleteBooks_included_hpp
#define commands_DeleteBooks_included_hpp

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

#include "src/dal/book.hpp"

namespace commands{
    
struct DeleteBooks {
    typedef void result_type;
    const std::unordered_map<std::string, bl::Book>& books_to_delete_;
    DeleteBooks(std::unordered_map<std::string, bl::Book>& books_to_delete) : books_to_delete_(books_to_delete){ }
   
    result_type execute( );

};

}

#endif
