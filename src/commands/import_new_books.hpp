#ifndef commands_ImportNewBooks_included_hpp
#define commands_ImportNewBooks_included_hpp

#include <string>
#include <vector>
#include <utility>
#include <tuple>

#include "src/bl/book.hpp"
#include "src/dal/base.hpp"

namespace commands{
    struct BookSourceBase{
        virtual int count()const = 0;
        virtual std::tuple<bl::Book, std::vector<char>,
            std::vector<char> > get_book(int index)const = 0;
        virtual std::string get_book_file_name(int index) const = 0;
    };
struct ImportNewBooks {
    typedef int result_type;
    const BookSourceBase& source_;
    std::list<bl::Book> books_from_db_;
    bl::Book book_factory(int index);
    bool already_imported(const std::string book_file_name ) const;
    void delete_removed_books();
    
    ImportNewBooks(BookSourceBase& source) :
        source_(source){
    }
    
    result_type execute( );

};

}

#endif
