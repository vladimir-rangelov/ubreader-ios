//
//  FirstViewController.m
//  ubreader
//
//  Created by Vlado Rangelov on 3/4/15.
//  Copyright (c) 2015 codore. All rights reserved.
//


#import "ShelvesTableViewController.h"

#import "AppDelegate.h"
#import "src/commands/get_shelf_list.hpp"
#import "src/commands/add_new_shelf.hpp"
#import "src/commands/delete_shelf.hpp"
#import "src/commands/update_shelf.hpp"

#define textfieldtag 22

@interface ShelvesTableViewController  () <UITableViewDelegate, UITableViewDataSource, UISearchBarDelegate, UITextFieldDelegate>{
}
@end

@implementation ShelvesTableViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        soci::session sql(dal::soci_session_factory());
        commands::GetShelfList shelf_list(sql);
        shelf_list.execute();
        std::swap(shelf_list.shelfs_from_db_, shelves_);
        currentTableRow = 0;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setTitle:NSLocalizedString(@"Shelves",nil)];
    [self updateLeftSideButtons];
    [self updateRightSideButtons];
}

-(void) update_db_after_rearrange {

    soci::session sql_(dal::soci_session_factory(), true);
    int sort_order = 0;
    std::for_each(shelves_.begin(), shelves_.end(), [&sql_ ,&sort_order](bl::BookShelf& bs){
        bs.sort_order_ = sort_order++;
        commands::UpdateShelf updated_shelf(sql_, bs);
        updated_shelf.execute();
    });
}

#pragma mark - UITextField delegate
- (BOOL)textFieldShouldEndEditing:(UITextField*)textField {
    //[textField resignFirstResponder];
    NSString *text = [textField text];
    std::string str_text([text UTF8String]);
    if([text length]==0) {
        return YES;
    }
    
    if (currentTableRow -1 < shelves_.size()) {
        shelves_[currentTableRow - 1].name_ = str_text;
        soci::session sql_(dal::soci_session_factory(), true);
        commands::UpdateShelf updated_shelf(sql_, shelves_[currentTableRow - 1]);
        updated_shelf.execute();
    } else {
        bl::BookShelf added_shelf;
        added_shelf.sort_order_ = shelves_.size();
        added_shelf.name_ = str_text;
        commands::AddNewShelf new_shelf(added_shelf);
        new_shelf.execute();
        shelves_.push_back(added_shelf);
    }
    return YES;
}
- (void)textFieldDidEndEditing:(UITextField *)textField {
    [textField removeFromSuperview];
    [self.table_ reloadData];
}

- (BOOL)textFieldShouldReturn:(UITextField*) textField;
{
    NSString *text = [textField text];
    if( [text length]>0){
        [textField resignFirstResponder];
        return YES;
    } else{
        return NO;
    }
}

#pragma mark - Table view data source
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if ( self.editing) {
        return shelves_.size() + 1;
    }else {
        return shelves_.size() + 2;
    }
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 44;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {

    static NSString *listCellIdentifier = @"ListCell";
    UITableViewCell *cell = (UITableViewCell*)[tableView dequeueReusableCellWithIdentifier:listCellIdentifier];
    if (cell == nil){
        UITableViewCellStyle style = UITableViewCellStyleDefault;
        cell = [[UITableViewCell alloc] initWithStyle:style reuseIdentifier:listCellIdentifier];
    }
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    cell.userInteractionEnabled = YES;

    NSInteger index = indexPath.row;

    if (index == 0) {
        cell.textLabel.text = NSLocalizedString(@"All", nil);
        cell.textLabel.textColor = [UIColor blackColor];
    } else if (index == (shelves_.size() + 1)) {
        cell.textLabel.text = NSLocalizedString(@"+ Add new",nil);
        cell.textLabel.textColor = [UIColor blueColor];
    } else {
        NSString *item = [NSString stringWithUTF8String:shelves_[index - 1].name_.c_str()];
        cell.textLabel.text = item;
        cell.textLabel.textColor = [UIColor blackColor];
    }
    
    return cell;
}

-(BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    if (self.editing && indexPath.row != 0) {
        return YES;
    }
    return NO;
}

- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)sourceIndexPath toIndexPath:(NSIndexPath *)destinationIndexPath {
    int from_index = sourceIndexPath.row;
    int to_index = destinationIndexPath.row;
    if (from_index == to_index) {
        return;
    }
    if (to_index != 0) {
        bl::BookShelf temp;
        temp = shelves_[to_index - 1];
        shelves_[to_index - 1] = shelves_[from_index - 1];
        shelves_.erase(shelves_.begin() + from_index - 1);
        if (to_index > from_index) {
            shelves_.insert(shelves_.begin() + to_index - 2, temp);
        } else {
            shelves_.insert(shelves_.begin() + to_index, temp);
        }
        
        [self update_db_after_rearrange];
        
    }
    [self.table_ reloadData];
    
}

- (BOOL)tableView:(UITableView *)tableview shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath *)indexPath {
    return NO;
}

#pragma mark - navigation bar buttons selectors
-(void) updateLeftSideButtons {
    
    if (self.editing) {
   
        UIBarButtonItem * btnDone = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"Done",nil) style:UIBarButtonItemStylePlain target:self action:@selector(barLeftButtonDonePressed:)];
        
        [self.navigationItem setLeftBarButtonItem:btnDone];

    } else {
        UIBarButtonItem * btnEdit = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"Edit",nil) style:UIBarButtonItemStylePlain target:self action:@selector(onEdit:event:)];
        
        [self.navigationItem setLeftBarButtonItems:[NSArray arrayWithObjects:btnEdit, nil]];
    }
}
-(void) updateRightSideButtons {
    if (self.editing) {
        
        [self.navigationItem setRightBarButtonItem:nil];
        
    } else {
        UIBarButtonItem * btnDone = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"Done",nil) style:UIBarButtonItemStylePlain target:self action:@selector(barRightButtonDonePressed:)];
        
        [self.navigationItem setRightBarButtonItems:[NSArray arrayWithObjects:btnDone, nil]];
    }
}

- (void)onEdit:(id)sender event:(UIEvent*)event {
    self.editing = true;
    self.table_.allowsSelectionDuringEditing = YES;
    [self.table_ setEditing: YES animated: YES];
    
    [self updateLeftSideButtons];
    [self updateRightSideButtons];
    [self.table_ reloadData];
}

-(void)barLeftButtonDonePressed:(UIBarButtonItem*)btn {
    self.editing = false;
    self.table_.allowsSelectionDuringEditing = NO;
    [self.table_ setEditing: NO animated: YES];
    
    UIView *removeView;
    while((removeView = [self.table_ viewWithTag:textfieldtag]) != nil) {
        [removeView removeFromSuperview];
    }
    
    [self updateLeftSideButtons];
    [self updateRightSideButtons];
    [self.table_ reloadData];
 
}

-(void)barRightButtonDonePressed:(UIBarButtonItem*)btn {
    [self dismissViewControllerAnimated:YES completion:nil];
}

-(void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle
        forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        int index =indexPath.row;
        bl::BookShelf bs = shelves_[index - 1];
        commands::DeleteShelf deleted_shelf(bs);
        deleted_shelf.execute();
        shelves_.erase(shelves_.begin() + index - 1);
        [self.table_ reloadData];
    }
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [self.table_ cellForRowAtIndexPath:indexPath];
    NSInteger index = indexPath.row;

    if (self.editing == false || !indexPath) {
        cell.userInteractionEnabled = YES;
        return UITableViewCellEditingStyleNone;
    }
    if (index == 0) {
        cell.textLabel.textColor = [UIColor lightGrayColor];
        cell.userInteractionEnabled = NO;
        return UITableViewCellEditingStyleNone;
    } else {
        cell.userInteractionEnabled = YES;
        return UITableViewCellEditingStyleDelete;
    }
}

#pragma mark - Table view delegate
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [self.table_ cellForRowAtIndexPath:indexPath];
    int index = indexPath.row;
    currentTableRow = index;
    if ((self.editing && index != 0 ) ||
        (self.editing == false && index -1== shelves_.size())) {
        UIView *removeView;
        while((removeView = [tableView viewWithTag:textfieldtag]) != nil) {
            [removeView removeFromSuperview];
        }
        
        CGRect TextFieldFrame=CGRectMake(10, 0,320, [self tableView:nil heightForRowAtIndexPath:nil]);
        UITextField *textfield;
        textfield=[[UITextField alloc]initWithFrame:TextFieldFrame];
        textfield.tag=textfieldtag;
        textfield.delegate = self;
        if (index -1== shelves_.size()) {
          textfield.text = @"";
        } else {
            textfield.text =
                [NSString stringWithUTF8String:shelves_[index - 1].name_.c_str()];
        }
        [textfield becomeFirstResponder];
        cell.textLabel.text = @"";
        [cell.contentView addSubview:textfield];
    } else if (self.editing == false){
        if(index == 0){
            bl::BookShelf empty_obj;
            [m_delegate setNewBookShelf:empty_obj];
        } else {
            [m_delegate setNewBookShelf:shelves_[index - 1]];
        }
        [self dismissViewControllerAnimated:YES completion:nil];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (UIModalPresentationStyle) adaptivePresentationStyleForPresentationController: (UIPresentationController * ) controller {
    return UIModalPresentationNone;
}

-(void)dealloc{

}

@end
