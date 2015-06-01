#include <tuple>
#import <UIKit/UIKit.h>
#include "platform/ios/ubreader/ios_book_source.h"
#include "ePub3/container.h"
#import <ePub3/utilities/byte_stream.h>
//#include <boost/gil/gil_all.hpp>

namespace commands {
    
    std::vector<std::pair<std::string, std::string> > directory_enum_vector();

    std::vector<char> img_resize(const std::vector<char>& src, double scale_k){
        std::vector<char> result;
        NSData *data = [[NSData alloc] initWithBytesNoCopy:
                        (void*)src.data() length:src.size() freeWhenDone:false];
        UIImage* source_image = [[UIImage alloc] initWithData:data];
        double scale = source_image.size.width/scale_k;
        CGSize newSize = CGSizeMake(source_image.size.width/scale,source_image.size.height/scale);
        
        UIGraphicsBeginImageContext(newSize);
        [source_image drawInRect:CGRectMake(0,0,newSize.width,newSize.height)];
        UIImage* newImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();

        NSData *imageData = UIImagePNGRepresentation(newImage);
        std::size_t length = [imageData length] / sizeof(char);
        char* array = ( char*) [imageData bytes];
        result.resize(length);
        std::copy_n(array, length, result.data());
        return result;
    }
    
    iOSBookSource::iOSBookSource(){
        books_uri_ = directory_enum_vector();
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        //NSString *documentsDirectory = [paths objectAtIndex:0];
        NSString *documentsDirectory = [paths lastObject];
        documents_root_directory_ = [documentsDirectory UTF8String];
    }
    
    int iOSBookSource::count()const{
        return books_uri_.size();
    }
    
    std::string iOSBookSource::get_book_file_name(int index) const {
        std::string fn = books_uri_[index].second;
        fn.erase(0, documents_root_directory_.length());
        return fn;
    }
    
    std::tuple<bl::Book, std::vector<char>, std::vector<char> >
        iOSBookSource::get_book(int index)const{
        std::tuple<bl::Book, std::vector<char>,
            std::vector<char> > result;
        
        std::get<0>(result).file_name_ = get_book_file_name(index);
        std::get<0>(result).mime_type_ = books_uri_[index].first;
        //if no title printing file_name;
        std::get<0>(result).title_ =  std::get<0>(result).file_name_;
            
        if (std::get<0>(result).mime_type_ .find("epub")!= std::string::npos) {
       
            ePub3::ContainerPtr container;
            container = ePub3::Container::OpenContainer(books_uri_[index].second);
            // ePub3::Container::PackageList packageList = container;
            ePub3::PackagePtr pkg = container->DefaultPackage();
            if(pkg.get() == nullptr) {
                std::string err("book with null default package. File name: ");
                err   += books_uri_[index].second;
                std::cout << err;
                throw std::runtime_error(err);
            }
            std::get<0>(result).title_ = pkg->Title().stl_str();
            std::get<0>(result).authors_ = pkg->Authors().stl_str();

            /*
            ePub3::string s = ePub3::string("cover.xhtml");
            */
            ePub3::ManifestItemPtr manifestItem;
            typedef std::map<ePub3::string,
                std::shared_ptr<ePub3::ManifestItem> > ManifestMap;
            const ManifestMap& manifest_map =
                pkg->Manifest();
                for(ManifestMap::const_iterator it =
                    manifest_map.begin(); it != manifest_map.end(); ++it){
                    ePub3::string id_ =it->first.tolower();
                    if (id_.find("cover") != std::string::npos) {
                        manifestItem = it->second;
                        ePub3::string href =manifestItem->Href();
                        ePub3::string mime_t = manifestItem->MediaType();
                        if (mime_t.find("image") != std::string::npos) {
                            break;
                        }
                    }
                }

            if (manifestItem != nullptr) {
                std::unique_ptr<ePub3::ByteStream> byteStream = pkg->ReadStreamForRelativePath(manifestItem->Href());
                
                if (byteStream != nullptr) {
                    std::get<0>(result).cover_mime_type_ =
                        manifestItem->MediaType().stl_str();
                    std::size_t length =  byteStream->BytesAvailable();
                    std::vector<char> raw_img_data;
                    raw_img_data.resize(length);
                    byteStream->ReadBytes(raw_img_data.data(), length);
                    std::get<1>(result) = img_resize(raw_img_data, 200.0);
                    std::get<2>(result) = img_resize(raw_img_data, 50.0);
                }
            }
        }
        /*
        else {
            NSMutableString *sss;
            sss=[[NSMutableString  alloc] initWithString: [[NSBundle mainBundle] resourcePath]];
            [sss appendString:@"/"] ;
            [sss appendString:@"no_cover.png"];
            NSData *imageData = [NSData dataWithContentsOfFile:sss];
            result.first.cover_mime_type_ = "image/png";
            std::size_t length = [imageData length] / sizeof(char);
            char* array = ( char*) [imageData bytes];
            result.second.resize(length);
            std::copy_n(array, length, result.second.data());
        }
       */
        return result;
    }
    
    std::vector<char> iOSBookSource::get_cover(int index)const{
        return {'a','b','c'};
    }
    
    NSDirectoryEnumerator * list_directory_enum( NSString *documentsDirectory) {
        NSDirectoryEnumerator *directoryContentEnum =
        [[NSFileManager defaultManager] enumeratorAtPath: documentsDirectory];
        
        return directoryContentEnum;
    }
    
    std::vector<std::pair<std::string, std::string> > directory_enum_vector(){
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        
        NSDirectoryEnumerator *book_directory = list_directory_enum(documentsDirectory);
        NSString *fileName, *fileType, *fileExtension;
        NSDictionary *fileAttributes;
        std::vector<std::pair<std::string, std::string> > result;
        while ((fileName = [book_directory nextObject])) {
            fileAttributes = [book_directory fileAttributes];
            fileType = [fileAttributes fileType];
            fileExtension = [fileName.pathExtension lowercaseString];
            if ( (fileType != NSFileTypeDirectory) &&
                ([fileExtension isEqualToString:@"pdf"] ||
                 [fileExtension isEqualToString:@"txt"] ||
                 [fileExtension isEqualToString:@"htm"] ||
                 [fileExtension isEqualToString:@"html"] ||
                 [fileExtension isEqualToString:@"jpg"] ||
                 [fileExtension isEqualToString:@"epub"] )) {
                NSString* full_path =
                    [documentsDirectory stringByAppendingString:@"/"];
                full_path =
                    [full_path stringByAppendingString:fileName];
                    result.push_back(
                        std::pair<std::string, std::string>(
                                                            std::string("application/")+[fileExtension UTF8String],[full_path UTF8String]));
                }
        }
        return result;
    }
}