//
//  AppDelegate.h
//  ubreader
//
//  Created by Vlado Rangelov on 3/4/15.
//  Copyright (c) 2015 codore. All rights reserved.
//

#import <UIKit/UIKit.h>

//@class VirtualLibraryViewController;

@interface AppDelegate : UIResponder <UIApplicationDelegate>{

}

@property (strong, nonatomic) UIWindow *window;
@property (nonatomic, retain) UINavigationController *navigationController;
@property (nonatomic, retain)  UIViewController* rootViewController;

- (void) open_book:(NSString*) path;
-(CGRect) getMainWindowFrame;

@end

