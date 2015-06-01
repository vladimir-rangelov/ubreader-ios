#import <UIKit/UIKit.h>
#include "src/dal/base.hpp"
//#include <soci-mysql.h>

namespace dal{
    
soci::connection_pool& soci_session_factory() {
     static soci::connection_pool pool(soci_connection_pool_size);
     return pool;
    /*
    soci::sqlite3_session_backend * sessionBackEnd =
    static_cast<soci::sqlite3_session_backend *>(sql.get_backend());
    sqlite_api::sqlite3_busy_timeout(sessionBackEnd->conn_, 1000);
    */
}
    
std::string db_file_name() {
    
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory , NSUserDomainMask, YES);
        NSString *documentsDir = [paths objectAtIndex:0];
        NSString* result = [documentsDir stringByAppendingPathComponent:@"_books_cache.sqlite"];
        std::string str =[result UTF8String];
        return str;
}

NSArray * list_directory_array() {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSArray *directoryContent =
        [[NSFileManager defaultManager] contentsOfDirectoryAtPath: documentsDirectory error: nil];
    
    return directoryContent;
}
    
NSDirectoryEnumerator * list_directory_enum() {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSDirectoryEnumerator *directoryContentEnum =
        [[NSFileManager defaultManager] enumeratorAtPath: documentsDirectory];
    
    return directoryContentEnum;
}

    
void init_db_pool() {
    soci::connection_pool &pool = soci_session_factory();
    std::string data_base_file = db_file_name();
    for (size_t i = 0; i != soci_connection_pool_size; ++i)
    {
        soci::session & sql = pool.at(i);
        sql.open(soci::sqlite3, data_base_file);
    }
}
    
    
std::string get_resource_file_as_str(std::string f_name ){
    
        NSString * file_name =
            [[NSString alloc] initWithUTF8String:f_name.c_str()];
        NSBundle *b = [NSBundle mainBundle];
        NSString *dir = [b resourcePath];
        NSArray *parts = [NSArray arrayWithObjects:
                          dir, /*@"doc"*/@"", file_name, (void *)nil];
        NSString *path = [NSString pathWithComponents:parts];
      //  NSString *myFile = [b pathForResource: @"drop_tables" ofType: @"txt"];
        const char *cpath = [path fileSystemRepresentation];
        std::string file(cpath);
        std::ifstream f_stream(file);
        std::stringstream result;
        result << f_stream.rdbuf();
        return result.str();
    }
    
}
