//
//  main.m
//  ubreader
//
//  Created by Vlado Rangelov on 3/4/15.
//  Copyright (c) 2015 codore. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        /*NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        NSDirectoryEnumerator *directoryContentEnum =
        [[NSFileManager defaultManager] enumeratorAtPath: documentsDirectory];
        
        NSString *filename;
        while ((filename = [directoryContentEnum nextObject])) {
            NSLog (@"file!  %@", filename);
        }*/
        
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
