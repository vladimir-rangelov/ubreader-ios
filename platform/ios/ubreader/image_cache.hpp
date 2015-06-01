#ifndef IMAGE_CACHE_INCLUDED
#define IMAGE_CACHE_INCLUDED


#include <list>
#include <utility>
#include "boost/uuid/uuid.hpp"

#import <UIKit/UIKit.h>

class ImageCache {
public:
    ImageCache() : size_(20) {}
    ImageCache(int size) : size_(size) {}

    void add(boost::uuids::uuid current_book_id ,UIImage *img ) {
        cache_.push_back(std::pair<boost::uuids::uuid , UIImage * >
                         (current_book_id, img));
        remove_unnecessarily_items();
        
    }
    UIImage* get(boost::uuids::uuid current_book_id ) {
        
        for (auto n : cache_) {
            if (n.first == current_book_id) {
                return n.second;
            };
        }
        
        return nullptr;
    }
    
    void set_size(int size) {
        size_ = size;
    }
    int get_size() {
        return size_;
    }
    
private:
    void remove_unnecessarily_items() {
        while(cache_.size() > size_) {
            cache_.front().second = nullptr;
            cache_.pop_front();
        }
    }
    
    std::list<std::pair<boost::uuids::uuid , UIImage * > > cache_;
    int size_;
    
};

#endif