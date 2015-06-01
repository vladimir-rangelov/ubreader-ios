//
//  ubreader
//
//  Created by Vlado Rangelov on 3/4/15.
//  Copyright (c) 2015 codore. All rights reserved.
//
#include <vector>
#include <unordered_map>
#include "src/bl/book.hpp"
#include "src/bl/book_shelf.hpp"
#include "src/commands/get_book_list.hpp"
#include "image_cache.hpp"

#import <UIKit/UIKit.h>
#import <QuickLook/QuickLook.h>
#import "DirectoryWatcher.h"
#import "MHWDirectoryWatcher.h"

@interface VirtualLibraryTableViewController : UIViewController<
                QLPreviewControllerDataSource,
                QLPreviewControllerDelegate,
                DirectoryWatcherDelegate>{
    std::vector<bl::Book> books_;
    int shelf_row_items_number_;
    bool isPortraitOr_;
    dal::Book::SortOrder book_sort_order_;
    ImageCache cache_;
    bool needs_import_;
    NSString* qlPreviewControllerFileName_;
    bool select_mode_;
    std::unordered_map<std::string, bl::Book> selected_book_ids_;
    UIPopoverController* optionsPopOver_;
    bl::BookShelf selected_shelf_;
}

@property (nonatomic, retain) IBOutlet UITableView *table_;
@property IBOutlet UISearchBar *searchBar;
@property IBOutlet UIBarButtonItem *switch_view;
@property IBOutlet UIBarButtonItem *shelves_button;
@property (nonatomic, strong) DirectoryWatcher *docWatcher;
@property (nonatomic, strong) MHWDirectoryWatcher *docWatcher2;

- (IBAction)onChangeView:(id)sender;
- (IBAction)onImport:(id)sender;
- (IBAction)onSortBy:(id)sender event:(UIEvent*)event;
- (IBAction)onAdd:(id)sender event:(UIEvent*)event;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;

@end

