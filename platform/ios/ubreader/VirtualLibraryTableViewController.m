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
#include "src/commands/get_book_list.hpp"
#include "src/commands/open_book.hpp"
#include "src/commands/delete_books.hpp"
#include "src/commands/move_books.hpp"
#include "platform/ios/ubreader/ios_book_source.h"
#include "src/dal/book.hpp"
#include "src/dal/id.hpp"
#include "src/dal/book_cover_attribute.hpp"

#import "VirtualLibraryTableViewController.h"
#import "SWRevealViewController.h"
#import "ShelvesTableViewController.h"
#import "BookResourceServer.h"
#import "HTTPServer.h"
#import "BookResourceConnection.h"

#import "AppDelegate.h"

std::future<int> g_f2_;

struct BookCoverAttributeMemAlloc {
    
    BookCoverAttributeMemAlloc(): raw_data_(nullptr), raw_data_size_(0) { }
    char* raw_data_;
    std::size_t raw_data_size_;
    void resize_raw_data(std::size_t s){
        raw_data_size_ = s;
        raw_data_ = (char*)malloc(raw_data_size_);
    }
    char* raw_ptr(){
        return raw_data_;
    }
};

@interface BookListTableViewCell : UITableViewCell
@end
@implementation BookListTableViewCell
- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    // ignore the style argument and force the creation with style UITableViewCellStyleSubtitle
    return [super initWithStyle:UITableViewCellStyleSubtitle
                reuseIdentifier:reuseIdentifier];
}
@end

@interface BookShelfTableViewCell : UITableViewCell {
@public  int shelf_row_items_number_;
@public  std::vector<std::size_t> book_indices;
};
@end
@implementation BookShelfTableViewCell
- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    return [super initWithStyle:UITableViewCellStyleSubtitle
                reuseIdentifier:reuseIdentifier];

}
@end

UIImage* loadCoverSync(boost::uuids::uuid current_book_id ){

    BookCoverAttributeMemAlloc ba_async;
    soci::session sql_async(dal::soci_session_factory());
    dal::BookCoverAttribute book_cover_att(sql_async);
    book_cover_att.get(ba_async, current_book_id);
    UIImage* cover_img;
    if(ba_async.raw_data_size_ > 0 ) {
        NSData* local_data2 =([[NSData alloc]
                               initWithBytesNoCopy:ba_async.raw_data_
                               length:ba_async.raw_data_size_
                               freeWhenDone:YES]);
        
        cover_img = [UIImage imageWithData:local_data2];
    } else {
        cover_img = [UIImage imageNamed:@"no_cover.png"];
    }
    return cover_img;
}

@interface VirtualLibraryTableViewController  () <          UITableViewDelegate,
    UITableViewDataSource,
    UISearchBarDelegate,
    ChangeBookShelfDelegate>{

@private NSTimer *timer_;
@private bool shelf_view_;
@private NSString* lastSearch_;
}
@end

@implementation VirtualLibraryTableViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        book_sort_order_ = dal::Book::RECENTLY_OPEN;
        shelf_view_ = true;
        select_mode_ = false;
    }
    return self;
}

-(bool) isActiveShelfView {
    return shelf_view_;
}

#pragma mark - Object life cycle
- (void)viewDidLoad {
    [super viewDidLoad];
    [self updateTitle];
    if(([[UIDevice currentDevice] orientation] == UIDeviceOrientationLandscapeLeft) ||
       ([[UIDevice currentDevice] orientation] == UIDeviceOrientationLandscapeRight))
    {
        isPortraitOr_ = false;
    }
    else
    {
        isPortraitOr_ = true;
    }
    [self orientationChanged];
    
    //self.docWatcher = [DirectoryWatcher watchFolderWithPath:[self applicationDocumentsDirectory] delegate:self];

     self.docWatcher2 = [MHWDirectoryWatcher directoryWatcherAtPath:[self applicationDocumentsDirectory] callback:^{
        // Actions which should be performed when the files in the directory
        //[self doSomethingNice];
         [self directoryDidChange:nil];
    }];
    lastSearch_ = @"";
    [self update_books_list];
    
    [self directoryDidChange:nil];
    [self updateLeftSideButtons];
    [self updateRightSideButtons];
    
    if(shelf_view_){
        self.table_.allowsSelection = NO;
    }
}

- (void)viewDidUnload
{
    self.docWatcher = nil;
    self.docWatcher2 = nullptr;
}
-(void)updateTitle{
    NSString* title = @"All Books";
    
    if(selected_shelf_.name_.size()) {
        title = [[NSString alloc]initWithUTF8String:selected_shelf_.name_.c_str()];
    }
    [self setTitle:title];
}

-(void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)orientation duration:(NSTimeInterval)duration {
    if (UIInterfaceOrientationIsLandscape(orientation)) {
        if(isPortraitOr_) {
            isPortraitOr_ = false;
            [self orientationChanged];
        }
    } else {
        if(!isPortraitOr_) {
            isPortraitOr_ = true;
            [self orientationChanged];
        }
    }
    [self.table_ reloadData];
}

#pragma mark - navigation bar buttons selectors
-(void) updateLeftSideButtons {

    if(self->select_mode_) {
        UIBarButtonItem * btnMove = [[UIBarButtonItem alloc] initWithTitle:@"Move " style:UIBarButtonItemStylePlain target:self action:@selector(barButtonMovePressed:event:)];


        UIBarButtonItem * btnDelete = [[UIBarButtonItem alloc] initWithTitle:@" Delete" style:UIBarButtonItemStylePlain target:self action:@selector(barButtonDeletePressed:event:)];
        
        [btnDelete setTitleTextAttributes:
         [NSDictionary dictionaryWithObjectsAndKeys:
          [UIColor colorWithRed:1
                          green:.3
                           blue:.3
                          alpha:1.0],
          NSForegroundColorAttributeName,nil]
                                      forState:UIControlStateNormal];
        [btnDelete setTitleTextAttributes:
         [NSDictionary dictionaryWithObjectsAndKeys:
          [UIColor colorWithRed:.8
                          green:.8
                           blue:.8
                          alpha:1.0],
          NSForegroundColorAttributeName,nil]
                                 forState:UIControlStateDisabled];
        
        
        if(selected_book_ids_.size() == 0){
            [btnMove setEnabled:NO];
            [btnDelete setEnabled:NO];
        }
        
        [self.navigationItem setLeftBarButtonItems:[NSArray arrayWithObjects:btnMove, btnDelete, nil]];
    } else {
        
        /*
        UIBarButtonItem *btnSideBar = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemBookmarks target:self action:@selector(barButtonSideBarPressed:)];
     
        
        [self.navigationItem setLeftBarButtonItems:[NSArray arrayWithObjects: btnSideBar, nil]];
 */
       /*
        UIImage *image = [UIImage imageNamed:@"side_bar_button.png"];
        
        UIBarButtonItem *btnSideBar = [[UIBarButtonItem alloc] initWithImage:image style:UIBarButtonItemStylePlain target:self action:@selector(barButtonSideBarPressed:)];
        [self.navigationItem setLeftBarButtonItems:[NSArray arrayWithObjects: btnSideBar, nil]];
        */
        SWRevealViewController *revealController = self.revealViewController;
        
        [self.navigationController.navigationBar addGestureRecognizer:revealController.panGestureRecognizer];
        
        UIBarButtonItem *revealButtonItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"reveal-icon.png"]
                                                                             style:UIBarButtonItemStylePlain target:revealController action:@selector(revealToggle:)];
        
        //self.navigationItem.leftBarButtonItem = revealButtonItem;
        [self.navigationItem setLeftBarButtonItems:[NSArray arrayWithObjects: revealButtonItem, nil]];
    }

}
-(void) updateRightSideButtons {
     if(self->select_mode_) {
         
    UIBarButtonItem * btnSelectAllOrNone;
    
         if (selected_book_ids_.size() == books_.size()) {
             btnSelectAllOrNone =
             [[UIBarButtonItem alloc] initWithTitle:@"Select None "
                                              style:UIBarButtonItemStylePlain target:self
                                             action:@selector(barButtonSelectNonePressed:)];

         } else {
             btnSelectAllOrNone =
             [[UIBarButtonItem alloc] initWithTitle:@"Select All "
                                             style:UIBarButtonItemStylePlain target:self
                                        action:@selector(barButtonSelectAllPressed:)];
         }
    
    UIBarButtonItem * btnDone =
            [[UIBarButtonItem alloc] initWithTitle:@"Done"
                                             style:UIBarButtonItemStylePlain
                                            target:self                                  action:@selector(barButtonDonePressed:)];
    

    [btnDone setTitleTextAttributes:@{NSFontAttributeName: [UIFont fontWithName:@"Helvetica-Bold" size:18.0],
                                         NSForegroundColorAttributeName: [UIColor blueColor]
                                         } forState:UIControlStateNormal];
    
    [self.navigationItem setRightBarButtonItems:[NSArray arrayWithObjects: btnDone,btnSelectAllOrNone, nil]];
    
     
     } else {
     
         
         UIBarButtonItem * btnSelect =
         [[UIBarButtonItem alloc] initWithTitle:@"Select"
                                          style:UIBarButtonItemStylePlain
                                         target:self
                                         action:@selector(barButtonSelectPressed:)];
         
         [self.navigationItem setRightBarButtonItems:[NSArray arrayWithObjects: btnSelect, nil]];
     }
}

-(void)barButtonMovePressed:(UIBarButtonItem*)btn event:(UIEvent*)event {
    [self onShowCollections:event withUpArrow:true];
}

-(void)barButtonDeletePressed:(UIBarButtonItem*)btn event:(UIEvent*)event{
    
    NSString *deleteStr = NSLocalizedString(@"Delete This Copy", nil);
    if (selected_book_ids_.size()>1) {
        deleteStr = NSLocalizedString(@"Delete These Copies", nil);
    }
    
    NSString *cancelButtonTitle = NSLocalizedString(@"Cancel", nil);
    
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    
    // Create the actions.
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:cancelButtonTitle style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        NSLog(@"The \"Okay/Cancel\" alert action sheet's cancel action occured.");
    }];
    
    UIAlertAction *deleteAction = [UIAlertAction actionWithTitle:deleteStr style:UIAlertActionStyleDestructive handler:^(UIAlertAction *action) {
        
        
        
        self->select_mode_ = false;
        self.shelves_button.enabled = YES;
        [self updateLeftSideButtons];
        [self updateRightSideButtons];
        
        commands::DeleteBooks db(selected_book_ids_);
        db.execute();
        [self onImport:nil];
    }];
    
    // Add the actions.
    [alertController addAction:cancelAction];
    [alertController addAction:deleteAction];

    
    UIPopoverPresentationController *popoverPresentationController = [alertController popoverPresentationController];
    if (popoverPresentationController) {
        UIView* view = [[event.allTouches anyObject] view];
        popoverPresentationController.sourceRect = [view convertRect:view.bounds toView:nil];
        popoverPresentationController.sourceView = self.view;
        popoverPresentationController.permittedArrowDirections = UIPopoverArrowDirectionUp;
    }
    
    [self presentViewController:alertController animated:YES completion:nil];
    
}

-(void)barButtonSelectAllPressed:(UIBarButtonItem*)btn {
    selected_book_ids_.clear();
    std::for_each(books_.begin(),
                  books_.end(),
                  [self](const bl::Book& b){
        self->selected_book_ids_[dal::convert::id(b.id())] = b;
    });
    [self.table_ reloadData];
    [self updateRightSideButtons];
    [self updateLeftSideButtons];
}

-(void)barButtonSelectNonePressed:(UIBarButtonItem*)btn {
    selected_book_ids_.clear();
    [self.table_ reloadData];
    [self updateRightSideButtons];
    [self updateLeftSideButtons];
}

-(void)barButtonSideBarPressed:(UIBarButtonItem*)btn {
    NSLog(@"button tapped %@", btn.title);
}

-(void)barButtonDonePressed:(UIBarButtonItem*)btn {
    select_mode_ = false;
    self.shelves_button.enabled = YES;
    [self updateRightSideButtons];
    [self updateLeftSideButtons];
   // self.table_.allowsMultipleSelectionDuringEditing = NO;
   // [self.table_ setEditing:NO animated:YES];
    
    if (selected_book_ids_.size()) {
        selected_book_ids_.clear();
        [self.table_ reloadData];
    }

   [self updateTitle];
    
}

-(void)barButtonSelectPressed:(UIBarButtonItem*)btn {
    select_mode_ = true;
    self.shelves_button.enabled = NO;
    [self updateRightSideButtons];
    [self updateLeftSideButtons];
    selected_book_ids_.clear();
    
    if (!shelf_view_) {
        //self.table_.allowsMultipleSelectionDuringEditing = YES;
        //[self.table_ setEditing:YES animated:YES];
    }
    
    if (IS_IPAD == false)
        [self setTitle:@""];
    
}


-(void) orientationChanged {
    if (isPortraitOr_) {
        shelf_row_items_number_ = 4;
    } else {
        shelf_row_items_number_ = 5;
    }
}
int import() {
    commands::iOSBookSource imp;
    auto clean_db = []()
    {
        soci::session sql(dal::soci_session_factory(), true);
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
    };
    //clean_db();
    commands::ImportNewBooks imp_book(imp);
    return imp_book.execute();
}
- (IBAction)onSortBy:(id)sender event:(UIEvent*)event{

    NSString *alertTitle = NSLocalizedString(@"Sort Books by", nil);
    
    NSString *titleStr = NSLocalizedString(@"Title", nil);
    NSString *authorStr = NSLocalizedString(@"Author", nil);
    NSString *recentlyOpen = NSLocalizedString(@"Recently Opened", nil);
    NSString *recentlyAdded = NSLocalizedString(@"Recently Added", nil);
    NSString *cancelStr = NSLocalizedString(@"Cancel", nil);
    
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:alertTitle message:nullptr preferredStyle:UIAlertControllerStyleActionSheet];
    
    // Create the actions.
  
    UIAlertAction *titleButton = [UIAlertAction actionWithTitle:titleStr style: ((book_sort_order_ != dal::Book::TITLE)?UIAlertActionStyleDefault: UIAlertActionStyleDestructive) handler:^(UIAlertAction *action) {
        [self sortOrderChenged:dal::Book::TITLE];
    }];
    
    UIAlertAction *authorButton = [UIAlertAction actionWithTitle:authorStr style:((book_sort_order_ != dal::Book::AUTHOR)?UIAlertActionStyleDefault: UIAlertActionStyleDestructive) handler:^(UIAlertAction *action) {
        [self sortOrderChenged: dal::Book::AUTHOR];
    }];
    
    UIAlertAction *recentlyOpenButton = [UIAlertAction actionWithTitle:recentlyOpen style:((book_sort_order_ != dal::Book::RECENTLY_OPEN)?UIAlertActionStyleDefault: UIAlertActionStyleDestructive) handler:^(UIAlertAction *action) {
        [self sortOrderChenged: dal::Book::RECENTLY_OPEN];
    }];
    
    UIAlertAction *recentlyAddedButton = [UIAlertAction actionWithTitle:recentlyAdded style:((book_sort_order_ != dal::Book::RECENTLY_ADDED)?UIAlertActionStyleDefault: UIAlertActionStyleDestructive) handler:^(UIAlertAction *action) {
        [self sortOrderChenged: dal::Book::RECENTLY_ADDED];
    }];
    
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:cancelStr style:UIAlertActionStyleCancel handler:NULL];

    [alertController addAction:recentlyOpenButton];
    [alertController addAction:titleButton];
    [alertController addAction:authorButton];
    [alertController addAction:recentlyAddedButton];
    [alertController addAction:cancel];

    UIImage* imgMyImage = [UIImage imageNamed:@"sort_title.png"];
    [titleButton setValue: imgMyImage forKey:@"image"];
    
    imgMyImage = [UIImage imageNamed:@"sort_author.png"];
    [authorButton setValue: imgMyImage forKey:@"image"];
    
    imgMyImage = [UIImage imageNamed:@"sort_time_open.png"];
    [recentlyOpenButton setValue: imgMyImage forKey:@"image"];
    
    imgMyImage = [UIImage imageNamed:@"sort_time_import.png"];
    [recentlyAddedButton setValue: imgMyImage forKey:@"image"];
    
    UIPopoverPresentationController *popoverPresentationController = [alertController popoverPresentationController];
    if (popoverPresentationController) {
        UIView* view = [[event.allTouches anyObject] view];
        popoverPresentationController.sourceRect = [view convertRect:view.bounds toView:nil];
        popoverPresentationController.sourceView = self.view;
        popoverPresentationController.permittedArrowDirections = UIPopoverArrowDirectionUp;
    }
    
    [self presentViewController:alertController animated:YES completion:nil];
    
}

-(void) sortOrderChenged:(dal::Book::SortOrder) newSortOrder {

    if ( book_sort_order_ == newSortOrder) {
        return;
    }
    book_sort_order_ = newSortOrder;
   
    [self update_books_list];
}

- (IBAction)onAdd:(id)sender event:(UIEvent*)event{
    [self onShowCollections:event withUpArrow:false];
}

- (void)onShowCollections:(UIEvent*)event withUpArrow:(bool)up{
    
    ShelvesTableViewController *shelfController = [[ShelvesTableViewController alloc]initWithNibName:@"ShelfTableViewController" bundle:nullptr];
    shelfController->m_delegate = self;
    shelfController->current_shelf_ = selected_shelf_;
    UINavigationController *navigationController = [[UINavigationController alloc] initWithRootViewController:shelfController];
    if (IS_IPAD) {
        optionsPopOver_ = [[UIPopoverController alloc] initWithContentViewController:navigationController];
        
        CGSize popOverS = CGSizeMake(320, 480);
        [optionsPopOver_ setPopoverContentSize:popOverS];
        
        UIView* view = [[event.allTouches anyObject] view];
        CGRect popOverRect = [view convertRect:view.bounds toView:nil];
        //popOverRect.origin.y -=self.bottom
        
        //bounds.size.height;
        [optionsPopOver_ presentPopoverFromRect:popOverRect inView:self.view permittedArrowDirections:up?UIPopoverArrowDirectionUp:UIPopoverArrowDirectionDown animated:YES];
    } else {
        [self presentViewController:navigationController animated:YES completion:nil];
    }
    
}


- (IBAction)onChangeView:(id)sender{
    
    
    shelf_view_ = !shelf_view_;
    if (shelf_view_) {
        self.table_.allowsSelection = NO;
        [((UIBarButtonItem*)sender) setImage:[UIImage imageNamed:@"toggle_view_list.png"]];
        
        if (self.table_.editing) {
            //self.table_.allowsMultipleSelectionDuringEditing = NO;
           // [self.table_ setEditing:NO animated:NO];
        }

        
    } else {
        self.table_.allowsSelection = YES;
        if (select_mode_) {
          //  self.table_.allowsMultipleSelectionDuringEditing = YES;
           // [self.table_ setEditing:YES animated:NO];
        } else {
            self.table_.allowsSelection = YES;
        }
        [((UIBarButtonItem*)sender) setImage:[UIImage imageNamed:@"toggle_view_grid.png"]];
    }
    
    [self.table_ reloadData];
}
- (IBAction)onImport:(id)sender{
    if ([timer_ isValid]==false) {
    
        [self setTitle:@"Importing..."];
        
        [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
        

        
    g_f2_ = std::async(std::launch::async, &import);
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
    if (is_ready(g_f2_)) {
        [timer invalidate];
        timer_ = nullptr;
        [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
        //[self.table_  reloadData];
        [self updateTitle];
        /*
        
        UIAlertView *theAlert = [[UIAlertView alloc] initWithTitle:@"Book Synchronization"
                                                           message:@"Done!"
                                                          delegate:self
                                                 cancelButtonTitle:@"OK"
                                                 otherButtonTitles:nil];
        [theAlert show];
        */
        
        int result = g_f2_.get();
        
        
        if(needs_import_){
            needs_import_ = false;
            [self directoryDidChange:nullptr];
        }
    }
    [self update_books_list];

    //[self.m_webView  reload];
}

-(void) loadCoverAsync:(boost::uuids::uuid) current_book_id
                  into: (UIButton *)btn
          withMimeType:(std::string) mimeType     {
    
    const std::string& book_id =
        dal::convert::id(current_book_id);

    if (selected_book_ids_.count(book_id) >0 ) {
        //btn.backgroundColor = [UIColor blueColor];
        UIImageView *selection_img = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"selection.png"]];
        double k_ = 0.4;
        if (IS_IPAD) {
            k_ = 0.2;
        }
        double k_y = k_*btn.frame.size.width/btn.frame.size.height;
        [selection_img setFrame:CGRectMake(btn.frame.size.width -btn.frame.size.width*k_, btn.frame.size.height - btn.frame.size.height*k_y, btn.frame.size.width*k_, btn.frame.size.height*k_y)];
        
        [btn addSubview:selection_img];
        btn.alpha = 0.7;
        
    } else {
        //btn.backgroundColor = [UIColor clearColor];
        btn.alpha = 1;
    }
    
    
    if (mimeType.find("epub") != std::string::npos) {
        UIImage* cacne_cover_img = self->cache_.get(current_book_id);
        if(cacne_cover_img!=nullptr){
            [btn setImage:cacne_cover_img forState:UIControlStateNormal];
            return;
        }
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^(void) {
            UIImage* cover_img = loadCoverSync(current_book_id);
            dispatch_sync(dispatch_get_main_queue(), ^(void) {
                self->cache_.add(current_book_id, cover_img);
                [btn setImage:cover_img forState:UIControlStateNormal];
            });
        });
    } else if (mimeType.find("pdf") != std::string::npos) {
        UIImage* cover_img = [UIImage imageNamed:@"no_cover_pdf.png"];
        [btn setImage:cover_img forState:UIControlStateNormal];
    } else {
        UIImage* cover_img = [UIImage imageNamed:@"no_cover_txt.png"];
        [btn setImage:cover_img forState:UIControlStateNormal];
    }

}


-(void) createShelfRowForCell:(BookShelfTableViewCell*) cell {
    for (UIView *view in cell.contentView.subviews) {
        [view removeFromSuperview];
    }
    
    float row_height = [self tableView:self.table_ heightForRowAtIndexPath:nullptr];
    float padding = 5;
    if (IS_IPAD) {
        padding = 10;
    }
    
    const float row_width = (self.table_.bounds.size.width -2*padding -1) / self->shelf_row_items_number_;
    
    for(std::size_t i=0;i < self->shelf_row_items_number_;++i) {
        
        if (cell->book_indices[i] > books_.size() -1) {
            continue;
        }
        
        UIButton *btn = [UIButton buttonWithType:UIButtonTypeCustom];
        btn.tag = cell->book_indices[i];
        btn.frame = CGRectMake(i*row_width + 2*padding, padding, row_width- 2*padding, row_height - 2*padding );
        [btn addTarget:self action:@selector(shelfBookTap:) forControlEvents:UIControlEventTouchUpInside];
        boost::uuids::uuid current_book_id = books_[btn.tag].id();
        [self loadCoverAsync:current_book_id
                        into:btn
                withMimeType:books_[btn.tag].mime_type_ ];
        btn.imageView.contentMode = UIViewContentModeBottom;
        [cell.contentView addSubview:btn];
        [[btn imageView] setContentMode: UIViewContentModeScaleAspectFit];
        
    }
}
- (void)shelfBookTap:(UIButton*)sender{
     [self onBookSelect:sender.tag];
}


-(BookListTableViewCell*) createBookListCell:(UITableView *)tableView forIndex:(NSInteger)index {
    static NSString *listCellIdentifier = @"ListCell";
    BookListTableViewCell *cell = (BookListTableViewCell*)[tableView dequeueReusableCellWithIdentifier:listCellIdentifier];
    if (cell == nil){
        UITableViewCellStyle style = UITableViewCellStyleDefault;
        cell = [[BookListTableViewCell alloc] initWithStyle:style reuseIdentifier:listCellIdentifier];
    }

    for (UIView *view in cell.contentView.subviews) {
        [view removeFromSuperview];
    }
    
    float row_height = [self tableView:self.table_ heightForRowAtIndexPath:nullptr];
    float padding_top_bottom = 2;
    float padding_left_right = 4;
    if (IS_IPAD) {
        padding_top_bottom = 5;
        padding_left_right = 10;
    }
    
    float row_width = (self.table_.bounds.size.width -2*padding_left_right -1) ;
    
    UITextField* title = [[UITextField alloc] initWithFrame:
                      CGRectMake(row_height + padding_left_right, 0.0,
                                 row_width, row_height/2)] ;
    title.font = [UIFont systemFontOfSize:20.0];
    title.textAlignment = NSTextAlignmentLeft;
    title.textColor = [UIColor darkGrayColor];
    
    title.text =[[NSString alloc] initWithUTF8String:
                 books_[index].title_.c_str()];
    title.contentVerticalAlignment = UIControlContentVerticalAlignmentBottom;
    title.userInteractionEnabled = NO;
    //title.adjustsFontSizeToFitWidth = YES;
    
    [cell.contentView addSubview:title];
    UITextField* authors = [[UITextField alloc] initWithFrame:
                        CGRectMake(row_height + padding_left_right, row_height/2,
                                   row_width, row_height/2)] ;
    authors.font = [UIFont systemFontOfSize:16.0];
    authors.textAlignment = NSTextAlignmentLeft;
    authors.textColor = [UIColor lightGrayColor];
    authors.text = [[NSString alloc] initWithUTF8String:
                    books_[index].authors_.c_str()];
    authors.contentVerticalAlignment = UIControlContentVerticalAlignmentTop;
    authors.userInteractionEnabled = NO;
    //[authors sizeToFit];
    [cell.contentView addSubview:authors];
    
    UIButton *btn = [UIButton buttonWithType:UIButtonTypeCustom];
    btn.frame = CGRectMake(2, padding_top_bottom,
                           row_height, row_height - padding_top_bottom*2 );
    btn.imageView.contentMode = UIViewContentModeBottom;
    [cell.contentView addSubview:btn];
    [[btn imageView] setContentMode: UIViewContentModeScaleAspectFit];
    //cell.imageView.image = loadCoverSync(books_[index].id());
    //loadCoverAsync(books_[index].id(), btn);
    [self loadCoverAsync:books_[index].id()
                    into:btn
            withMimeType:books_[index].mime_type_];
    return cell;
}


-(BookShelfTableViewCell*) createBookShelfCell:(UITableView *)tableView forIndex:(NSInteger)index {
    
    static NSString *listCellPortraitIdentifier = @"ShelfCellPortrait";
    static NSString *listCellLandscapeIdentifier = @"ShelfCellLandscape";

    NSString *listCellIdentifier = listCellLandscapeIdentifier;
    if (isPortraitOr_) {
        listCellIdentifier = listCellPortraitIdentifier;
    }
    
    BookShelfTableViewCell *cell = (BookShelfTableViewCell*)[tableView dequeueReusableCellWithIdentifier:listCellIdentifier];
    if (cell == nil){
        UITableViewCellStyle style = UITableViewCellStyleDefault;
        cell = [[BookShelfTableViewCell alloc] initWithStyle:style reuseIdentifier:listCellIdentifier];
    }
    cell->shelf_row_items_number_ = shelf_row_items_number_;
    cell->book_indices.clear();
    for(std::size_t i=0;i<shelf_row_items_number_;++i) {
        std::size_t actual_index = index*shelf_row_items_number_ + i;
        /*
        if(actual_index > books_.size()-1){
            actual_index = books_.size() -1;
        }
        */
        cell->book_indices.push_back(actual_index);
    }
    [self createShelfRowForCell:cell ];

    return cell;
}
-(void) onBookSelect:(int)book_index{
    
     bl::Book& book_to_open = books_[book_index];
    if(select_mode_) {
        const std::string& book_id =  dal::convert::id(book_to_open.id());
        std::unordered_map<std::string,
                bl::Book>::const_iterator it =
                    selected_book_ids_.find(book_id);
        if (it != selected_book_ids_.end()) {
            selected_book_ids_.erase(it);
        } else {
            selected_book_ids_[book_id] = book_to_open;
        }
        [self.table_ reloadData];
        [self updateLeftSideButtons];
        [self updateRightSideButtons];
    } else {
    
        //bl::Book book_to_open = books_[book_index];
        commands::OpenBook opened_book(book_to_open);
        opened_book.execute();
        
        if (book_to_open.mime_type_.find("epub")!= std::string::npos) {
            NSString *full_path = [self getBookFullPath:book_to_open];
            AppDelegate* app_delegate =  (AppDelegate*)[[UIApplication sharedApplication] delegate];
            [app_delegate open_book:full_path];

        } else {
       
            qlPreviewControllerFileName_ = [self getBookFullPath:book_to_open];
            
            QLPreviewController *previewController = [[QLPreviewController alloc] init];
            previewController.dataSource = self;
            previewController.delegate = self;
            
            // start previewing the document at the current section index
            previewController.currentPreviewItemIndex = book_index;
            [[self navigationController] pushViewController:previewController animated:YES];
        }
    }
}
#pragma mark - Table view data source
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    
    int not_full_rows = 0;
    if(books_.size()%shelf_row_items_number_ !=0){
        not_full_rows = 1;
    }
    
    if (shelf_view_) {
        return  books_.size()/shelf_row_items_number_ + not_full_rows;
    } else {
        return books_.size();
    }
}
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (shelf_view_) {
        if (IS_IPAD) {
            return 250;
        } else {
            return 120;
        }
    }
    else{
        if (IS_IPAD) {
            return 120;
        } else {
            return 80;
        }
    }
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (shelf_view_) {
       return [self createBookShelfCell:tableView
                               forIndex:indexPath.row];
    } else {
        return [self createBookListCell:tableView
                           forIndex:indexPath.row];
    }
}

-(NSString*) getBookFullPath:(bl::Book&) book{

    NSString* book_file_name =
        [[NSString alloc] initWithUTF8String:book.file_name_.c_str()];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *full_path = [NSString stringWithFormat:@"%@%@", [paths objectAtIndex:0], book_file_name];
    return full_path;
}

#pragma mark - Table view delegate
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    int book_index = indexPath.row;
    if (shelf_view_) {
         book_index *= shelf_row_items_number_;
    }
    [self onBookSelect:book_index];
}
#pragma mark - File system support

- (NSString *)applicationDocumentsDirectory
{
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];
}

- (void)directoryDidChange:(DirectoryWatcher *)folderWatcher
{
    if ([timer_ isValid]==false) {
        
        [self setTitle:@"Importing..."];
        
        [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
        
        
        
        g_f2_ = std::async(std::launch::async, &import);
        timer_ = [NSTimer scheduledTimerWithTimeInterval:1.0f
                                                  target:self
                                                selector:@selector(timerFired:)
                                                userInfo:nil
                                                 repeats:YES];
    } else {
        //needs_import_ = true;
    }
}

#pragma mark - UIDocumentInteractionControllerDelegate

- (UIViewController *)documentInteractionControllerViewControllerForPreview:(UIDocumentInteractionController *)interactionController
{
    return self;
}


#pragma mark - QLPreviewControllerDataSource

// Returns the number of items that the preview controller should preview
- (NSInteger)numberOfPreviewItemsInPreviewController:(QLPreviewController *)previewController
{
    
    return 1;
}

- (void)previewControllerDidDismiss:(QLPreviewController *)controller
{
    // if the preview dismissed (done button touched), use this method to post-process previews
}

// returns the item that the preview controller should preview
- (id)previewController:(QLPreviewController *)previewController previewItemAtIndex:(NSInteger)idx
{
    
    bl::Book book_to_open = books_[idx];
    NSString *full_path = [self getBookFullPath:book_to_open];
    //return full_path;
    NSURL *fileURL = [[NSURL alloc] initFileURLWithPath:qlPreviewControllerFileName_];

    return fileURL;
}

#pragma mark - UISearchBarDelegate
- (void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchText {
    if ([searchText length]>1) {

        [self searchChenged:searchText];
        
    } else if([searchText length] ==0){
        [self searchChenged:@""];
    }
}
- (BOOL)searchBarShouldBeginEditing:(UISearchBar *)searchBar{
    searchBar.showsCancelButton = YES;
    return YES;
}
- (void)searchBarCancelButtonClicked:(UISearchBar *)searchBar  {
    [searchBar resignFirstResponder];
    searchBar.showsCancelButton = NO;
}

-(void) searchChenged:(NSString*) searchText{
    
    if ([searchText isEqualToString:lastSearch_]) {
        return;
    }
    lastSearch_ = searchText;
    [self update_books_list];

}

- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar {
    [searchBar resignFirstResponder];
    searchBar.showsCancelButton = NO;
}



-(void) update_books_list{
    

    dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^(void) {
        soci::session sql(dal::soci_session_factory());
        std::string search = [lastSearch_ UTF8String];
        commands::GetBookList get_list_command(sql, search, book_sort_order_, selected_shelf_.id(), true);
        get_list_command.execute();
        std::swap(books_, get_list_command.books_from_db_);
        dispatch_async(dispatch_get_main_queue(), ^{
            //books_ = get_list_command.books_from_db_;
            [self.table_ reloadData];
            /*
             [self.table_ setNeedsDisplay];
             NSIndexSet * sections = [NSIndexSet indexSetWithIndex:0];
             [self.table_ reloadSections:sections withRowAnimation:UITableViewRowAnimationNone];
             */
        });
    });

}

#pragma mark - ChangeBookShelfDelegate
- (void)setNewBookShelf:(bl::BookShelf& )bs {
    self.shelves_button.enabled = YES;
    select_mode_ = false;
    [self updateRightSideButtons];
    [self updateLeftSideButtons];

    selected_shelf_ = bs;
    if(selected_book_ids_.size()){
        commands::MoveBooks bm(selected_book_ids_,
                               selected_shelf_);
        bm.execute();
        selected_book_ids_.clear();
    }
    [self update_books_list];
    [self updateTitle];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)dealloc{

}

@end
