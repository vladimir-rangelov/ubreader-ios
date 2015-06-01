//
//  FirstViewController.h
//  ubreader
//
//  Created by Vlado Rangelov on 3/4/15.
//  Copyright (c) 2015 codore. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface VirtualLibraryViewController : UIViewController{

}

@property (nonatomic, retain) IBOutlet UIWebView *m_webView;

- (IBAction)onChangeView:(id)sender;
- (IBAction)onImport:(id)sender;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;
- (void)stopWebViewLoading;
@end

