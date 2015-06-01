//
//  FirstViewController.m
//  ubreader
//
//  Created by Vlado Rangelov on 3/4/15.
//  Copyright (c) 2015 codore. All rights reserved.
//
#include <future>
#include "src/dal/base.hpp"
#include "src/commands/import_new_books.hpp"
#include "platform/ios/ubreader/ios_book_source.h"
#include "src/dal/book.hpp"
#include "src/dal/id.hpp"

#import "VirtualLibraryViewController.h"
#import "BookResourceServer.h"
#import "HTTPServer.h"
#import "BookResourceConnection.h"

#import "AppDelegate.h"

std::future<void> g_f_;
@interface VirtualLibraryViewController  () <
BookResourceServerDelegate,
UIWebViewDelegate> {

@private BookResourceServer *m_resourceServer;
@private NSData *m_specialPayload_AnnotationsCSS;
@private NSData *m_specialPayload_MathJaxJS;
@private NSTimer *timer_;
@private bool shelf_view_;
}
@end

@implementation VirtualLibraryViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
    
        m_resourceServer = [[BookResourceServer alloc]
                            initWithDelegate:self
                            specialPayloadAnnotationsCSS:m_specialPayload_AnnotationsCSS
                            specialPayloadMathJaxJS:m_specialPayload_MathJaxJS];
        
        if (m_resourceServer == nil) {
            return nil;
        }

    
        shelf_view_ = false;
    }
    return self;
}

- (void)initializeSpecialPayloads {
    
    // May be left to NIL if desired (in which case MathJax and annotations.css functionality will be disabled).
    
    m_specialPayload_AnnotationsCSS = nil;
    m_specialPayload_MathJaxJS = nil;
    
    
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"MathJax" ofType:@"js" inDirectory:@"mathjax"];
    if (filePath != nil) {
        NSString *code = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:nil];
        if (code != nil) {
            NSData *data = [code dataUsingEncoding:NSUTF8StringEncoding];
            if (data != nil) {
                m_specialPayload_MathJaxJS = data;
            }
        }
    }
    
    filePath = [[NSBundle mainBundle] pathForResource:@"annotations" ofType:@"css"];
    if (filePath != nil) {
        NSString *code = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:nil];
        if (code != nil) {
            NSData *data = [code dataUsingEncoding:NSUTF8StringEncoding];
            if (data != nil) {
                m_specialPayload_AnnotationsCSS = data;
            }
        }
    }
}

- (void) packageResourceServer:(BookResourceServer *)packageResourceServer
executeJavaScript:(NSString *)javaScript
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.m_webView stringByEvaluatingJavaScriptFromString:javaScript];
    });
}
-(bool) isActiveShelfView {
    return shelf_view_;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    [self setTitle:@"Book list"];
    //UIWebView *webView = [[UIWebView alloc] init];
    //self.m_webView = webView;
    self.m_webView.delegate = self;
   // webView.hidden = YES;
    //webView.scalesPageToFit = YES;
    //webView.scrollView.bounces = YES;
    //webView.allowsInlineMediaPlayback = YES;
    //webView.mediaPlaybackRequiresUserAction = NO;
    //[self.view addSubview:webView];
    //self.m_webView.frame = self.view.bounds;
    [self webviewReload];
    
}

-(void) webviewReload{
    int port = [m_resourceServer port];
    
    // NSURL *url = [[NSBundle mainBundle] URLForResource:@"book_library.html" withExtension:nil];
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:
                                       @"http://127.0.0.1:%d/", port]];
    [self.m_webView loadRequest:[NSURLRequest requestWithURL:url]];
}
void import() {
    commands::iOSBookSource imp;
    soci::session sql(dal::soci_session_factory());
    sql.begin();
    bool throw_flag = false;
    try{
    if(dal::books_table_exist(sql)){
        dal::drop_database(sql);
    }
    dal::create_all_tables(sql);
    } catch(...) {
        sql.rollback();
        throw_flag = true;
    }
    if(!throw_flag){
        sql.commit();
    }
    commands::ImportNewBooks imp_book(imp, sql);
    imp_book.execute();
}


- (IBAction)onChangeView:(id)sender{
   // shelf_view_ = !shelf_view_;
   // [self webviewReload];
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.m_webView stringByEvaluatingJavaScriptFromString:@"switch_view_type()"];
        });
        
}
- (IBAction)onImport:(id)sender{
    if ([timer_ isValid]==false) {
    
        [self setTitle:@"Importing..."];
        
        UIActivityIndicatorView *activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhite];
        UIBarButtonItem *itemIndicator = [[UIBarButtonItem alloc] initWithCustomView:activityIndicator];
        [self.navigationController.navigationItem setLeftBarButtonItem:itemIndicator];
        [activityIndicator startAnimating];
        

        
    g_f_ = std::async(std::launch::async, &import);
    timer_ = [NSTimer scheduledTimerWithTimeInterval:1.0f
                                              target:self
                                            selector:@selector(timerFired:)
                                            userInfo:nil
                                             repeats:YES];
    }
    //[self.m_webView  reload];
}

template<typename R>
bool is_ready(std::future<R> const& f)
{ return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
- (void)timerFired:(NSTimer *)timer {
    if (is_ready(g_f_)) {
        [timer invalidate];
        timer_ = nullptr;
        
        [self.m_webView  reload];
        [self setTitle:@"Book list"];
        
        
        UIAlertView *theAlert = [[UIAlertView alloc] initWithTitle:@"Book Synchronization"
                                                           message:@"Done!"
                                                          delegate:self
                                                 cancelButtonTitle:@"OK"
                                                 otherButtonTitles:nil];
        [theAlert show];
        

    }
    //[self.m_webView  reload];
}

- (void)stopWebViewLoading{

    [self.m_webView stopLoading];
};

#define BOOK_URL "open_book.html?book="
// web view delegate methods
- (BOOL)
webView:(UIWebView *)webView
shouldStartLoadWithRequest:(NSURLRequest *)request
navigationType:(UIWebViewNavigationType)navigationType
{
    NSString *url = request.URL.relativeString;
    std::string uri_str = [url UTF8String];
    
    if (uri_str.find(BOOK_URL) != std::string::npos) {
    
            std::size_t cover_url_pref_lenght = 0;
        do
           cover_url_pref_lenght ++;
        while (uri_str[cover_url_pref_lenght] != '=');
        cover_url_pref_lenght ++;
        std::string book_id(&uri_str[cover_url_pref_lenght],
                                uri_str.length() - cover_url_pref_lenght);
            soci::session sql(dal::soci_session_factory());

            dal::Book b(sql);
            bl::Book book_to_open =
            b.base_functions_.get( dal::convert::id(book_id));
            NSString* book_file_name = [[NSString alloc] initWithUTF8String:book_to_open.file_name_.c_str()];
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            NSString *full_path = [NSString stringWithFormat:@"%@%@", [paths objectAtIndex:0], book_file_name];
        
        
            AppDelegate* app_delegate =  (AppDelegate*)[[UIApplication sharedApplication] delegate];
            [app_delegate open_book:full_path];
        
        return NO;
    }
    
    return YES;
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error{

}
- (void)webViewDidFinishLoad:(UIWebView *)webView{

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)dealloc{

}

@end
