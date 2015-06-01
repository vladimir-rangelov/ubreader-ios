//
//  ubreader
//
//  Created by Vlado Rangelov on 3/4/15.
//  Copyright (c) 2015 codore. All rights reserved.
//
#include "src/commands/get_shelf_list.hpp"
#import <UIKit/UIKit.h>

@protocol ChangeBookShelfDelegate <NSObject>
- (void)setNewBookShelf:(bl::BookShelf& )bs;
@end


@interface ShelvesTableViewController : UIViewController {
    @public __weak id <ChangeBookShelfDelegate> m_delegate;
    std::vector<bl::BookShelf> shelves_;
    bl::BookShelf current_shelf_;
    int currentTableRow;
}

@property (nonatomic, retain) IBOutlet UITableView *table_;
- (void)onEdit:(id)sender event:(UIEvent*)event;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;

@end

