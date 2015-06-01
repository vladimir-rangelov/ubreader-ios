//
//  AppDelegate.m
//  ubreader
//
//  Created by Vlado Rangelov on 3/4/15.
//  Copyright (c) 2015 codore. All rights reserved.
//

#include "src/dal/base.hpp"
#import "AppDelegate.h"
#include "platform/ios/ubreader/ios_book_source.h"
#import "VirtualLibraryViewController.h"
#import "VirtualLibraryTableViewController.h"
#import "VirtualLibraryMenuViewController.h"
#import "SWRevealViewController.h"

#import <ePub3/container.h>
#import <ePub3/initialization.h>
#import "NavigationElementController.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

void check_database() {
    soci::session sql(dal::soci_session_factory());
    if(dal::books_table_exist(sql) == false){
        dal::create_all_tables(sql);
    }
}

void import_books(){
    commands::iOSBookSource imp;
    commands::ImportNewBooks imp_book(imp);
    imp_book.execute();
}

-(CGRect) getMainWindowFrame{
    CGRect r = self.window.bounds;
    /*if ([[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)] &&
        ([UIScreen mainScreen].scale == 2.0)) {
        r.size.width *= 2;
        r.size.height *= 2;
    }*/
    
    return r;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    dal::init_db_pool();
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    VirtualLibraryTableViewController* vtvc = nullptr;
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        vtvc = [[VirtualLibraryTableViewController alloc] initWithNibName:@"BookLibraryTableViewController_iPhone" bundle:nil];
    } else {
        vtvc = [[VirtualLibraryTableViewController alloc] initWithNibName:@"BookLibraryTableViewController_iPad" bundle:nil];
    }

   
    VirtualLibraryMenuViewController *menuViewController = [[VirtualLibraryMenuViewController alloc] initWithNibName:@"VirtualLibraryMenuViewController_" bundle:nil];

    UINavigationController *frontNavigationController = [[UINavigationController alloc] initWithRootViewController:vtvc];
   // UINavigationController *rearNavigationController = [[UINavigationController alloc] initWithRootViewController:menuViewController];
    
    SWRevealViewController *mainRevealController = [[SWRevealViewController alloc]
                                                    initWithRearViewController:menuViewController frontViewController:frontNavigationController];
    
    self.navigationController = frontNavigationController;
    self.navigationController.navigationBar.translucent= NO;
    
/*
    self.navigationController = [[UINavigationController alloc] initWithRootViewController:self.rootViewController];

    self.navigationController.view.frame = [[UIScreen mainScreen] bounds];
*/

    self.window.rootViewController = mainRevealController;
    [self.window addSubview:mainRevealController.view];
    [self.window makeKeyAndVisible];
//    [self.navigationController setNavigationBarHidden:YES animated:NO];

    ePub3::InitializeSdk();
    ePub3::PopulateFilterManager();
    
    check_database();
    //import_books();

    //[application  setStatusBarHidden:NO];
    //[application  setStatusBarStyle:UIStatusBarStyleDefault];
    return YES;
}
- (void) open_book:(NSString*) path {
    NavigationElementController *c = nil;
    c = [[NavigationElementController alloc]
         initWithPath:path];
//    [self.rootViewController stopWebViewLoading];
	[self.navigationController pushViewController:c animated:YES];
}

-(BOOL) application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    commands::iOSBookSource imp;
    commands::ImportNewBooks imp_book(imp);
    imp_book.execute();

    return true;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

void remove_book(const char *file)
{
    NSString* fileName =
        [[NSString alloc] initWithUTF8String:file];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *documentsPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    
    NSString *filePath = [documentsPath stringByAppendingPathComponent:fileName];
    NSError *error;
    BOOL success = [fileManager removeItemAtPath:filePath error:&error];
    if (success) {
    }
    else
    {
        NSLog(@"Could not delete file -:%@ ",[error localizedDescription]);
    }
}


@end
