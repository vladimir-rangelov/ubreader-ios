
#import "AppDelegate.h"
#import "BookResourceDataResponse.h"
#include "src/dal/book.hpp"
#include "src/dal/id.hpp"
#include "src/dal/book_cover_attribute.hpp"
#include <google/template.h>
#include <google/template_from_string.h>

#define COVER_URL "cover.png?book="
#define BOOK_URL "open_book.html?book="

NSString* local_resource_to_string(NSString *file_name){
    NSMutableString *ms;
    ms=[[NSMutableString  alloc] initWithString: [[NSBundle mainBundle] resourcePath]];
    [ms appendString:@"/"];
    [ms appendString:file_name];
    NSData * list_data = [NSData dataWithContentsOfFile:ms];
    NSString* tmp_list = [[NSString alloc] initWithData:list_data
                                               encoding:NSUTF8StringEncoding];
    return tmp_list;
}

void get_html_list_templates(std::string& page,
                             std::string& list_item,
                             bool shelfViewType){
  
    NSString* tmp_str;
    shelfViewType = true;
    if(shelfViewType) {
        tmp_str = local_resource_to_string(@"book_shelf.html");
    } else {
        tmp_str = local_resource_to_string(@"book_list.html");
    }
    page = [tmp_str UTF8String];
    
    if(shelfViewType) {
        tmp_str = local_resource_to_string(@"book_shelf_item.html");
    } else {
        tmp_str = local_resource_to_string(@"book_list_item.html");
    }
   
    list_item = [tmp_str UTF8String];
}
int get_screen_width(){
    AppDelegate* app_delegate = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    CGRect window_frame = [app_delegate getMainWindowFrame];
    return window_frame.size.width;
}

//std::mutex m;
std::string book_list_to_html(std::list<bl::Book> list,
                              bool shelfViewType){
    
    //std::lock_guard<std::mutex> lm(m);
    std::string page;
    std::string list_item;
    std::stringstream list_accumulator;
    get_html_list_templates(page, list_item, shelfViewType);
    google::TemplateDictionary dict("list_view");
    std::shared_ptr <google::Template> page_tpl;
    page_tpl.reset(google::TemplateFromString::GetTemplate("", page,
                                                                    google::DO_NOT_STRIP));
    std::shared_ptr <google::Template> list_item_tpl;
    list_item_tpl.reset(google::TemplateFromString::GetTemplate("", list_item,
                                                           google::DO_NOT_STRIP));
    
    int count = 0;
    for (std::list<bl::Book>::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
        
        std::string url = COVER_URL;
        url += dal::convert::id(*iter);

        dict.SetValue("COVER_URL", url);
        url = BOOK_URL;
        url += dal::convert::id(*iter);
        dict.SetValue("BOOK_URL",url);
        
        dict.SetValue("BOOK_TITLE", iter->title_);
        dict.SetValue("BOOK_AUTHORS", iter->authors_);
        dict.SetIntValue("ROW_NUMBER",count++);
        dict.SetValue("IMAGE_PARENT_ID", "_image_" + std::to_string(count));
        std::string tmp_text;
        list_item_tpl->Expand(&tmp_text, &dict);
        list_accumulator << tmp_text;
    }
    
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        dict.SetValue("DEVICE_TYPE", "iphone");
    } else {
        dict.SetValue("DEVICE_TYPE", "ipad");
    }

    dict.SetValue("BOOK_LIST", list_accumulator.str());
    std::string result;
    page_tpl->Expand(&result, &dict);
    return result;
}


@implementation BookResourceDataResponse

- (id)initWithUri:(NSString*)uri andActiveShelfViewType: (bool)shelfViewType{
    std::string uri_str = [uri UTF8String];
    soci::session sql(dal::soci_session_factory());
    dal::BookCoverAttribute book_cover_att(sql);
    dal::Book b(sql);
    NSData * local_data;

    auto print_book_list = [&b, &local_data, shelfViewType]() {
        std::list<bl::Book> b_list =b.base_functions_.rows();
        std::string html = book_list_to_html(b_list, shelfViewType);
        NSString * FALLBACK_HTML = [[NSString alloc] initWithUTF8String:html.c_str()];
        local_data = [FALLBACK_HTML dataUsingEncoding:NSUTF8StringEncoding];
    };
    
    if (uri_str.find(COVER_URL) != std::string::npos) {
        std::size_t cover_url_pref_lenght = sizeof(COVER_URL);
        std::string book_id(&uri_str[cover_url_pref_lenght],
                            uri_str.length() - cover_url_pref_lenght);
        bl::BookAttributes ba;
        book_cover_att.get(ba, dal::convert::id(book_id));
        if(ba.raw_data_.size() > 0 ) {
            local_data =([[NSData alloc]
                                             initWithBytes:ba.raw_data_.data()
                                             length:ba.raw_data_.size()]);
        } else {
            NSMutableString *sss;
            sss=[[NSMutableString  alloc] initWithString: [[NSBundle mainBundle] resourcePath]];
            [sss appendString:@"/no_cover.png"];
            local_data = [NSData dataWithContentsOfFile:sss];
        }
        self.contentType = @"image/png";
    }
   
    else if( (uri_str.find(".js") != std::string::npos) ||
           (uri_str.find(".css") != std::string::npos) ||
            (uri_str.find(".png") != std::string::npos)) {
        NSMutableString *sss;
        sss=[[NSMutableString  alloc] initWithString: [[NSBundle mainBundle] resourcePath]];
        [sss appendString:uri];
        local_data = [NSData dataWithContentsOfFile:sss];
        //self.contentType = @"text/javascript";
    }
    else {
        print_book_list();
        self.contentType = @"text/html";
    }
    
    if((self = [super initWithData:local_data]))
    {
    }
    return self;
}


- (NSDictionary *)httpHeaders {
	if (self.contentType) {
		return @{@"Content-Type": self.contentType};
	}
	else {
		return @{};
	}
}

@end
