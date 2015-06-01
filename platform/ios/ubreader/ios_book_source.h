#import <UIKit/UIKit.h>

#include <utility>
#include "src/commands/import_new_books.hpp"

namespace commands {
    struct iOSBookSource : public BookSourceBase{
        std::vector<std::pair<std::string, std::string> > books_uri_;
        std::string documents_root_directory_;
        iOSBookSource();
        virtual int count()const;
        virtual std::string get_book_file_name(int index)const;
        virtual std::tuple<bl::Book, std::vector<char>,
                std::vector<char> >
                get_book(int index)const;
        virtual std::vector<char> get_cover(int index)const;
    };

}