//
//  Wrapper.h
//  facerec
//
//  Created by Eugene G on 9/1/20.
//  Copyright © 2020 Eugene G. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface Wrapper : NSObject

- (instancetype)initWithPaths:(NSString*)facerecPath datasetPath:(NSString*)datasetPath;

- (UIImage *) processImage:(UIImage *)image;

@end

NS_ASSUME_NONNULL_END
