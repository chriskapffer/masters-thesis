//
//  ObjectTrackerLibrary.h
//  CKObjectTrackerShared
//
//  Created by Christoph Kapffer on 07.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>

#define MAX_RECORDED_FRAMES 10000

struct Homography
{
    CGFloat m11, m12, m13;
    CGFloat m21, m22, m23;
    CGFloat m31, m32, m33;
};

@protocol ObjectTrackerLibraryDelegate <NSObject>
// TODO: call these method from library
@required
- (void)trackedObjectWithHomography:(Homography)homography;
@optional
- (void)reachedDebugInfoRecordingLimit:(NSString*)debugInfo;
- (void)failedToTrackObjectInImage;
- (void)didProcessFrame;

@end

@interface ObjectTrackerLibrary : NSObject

@property (nonatomic, weak) id<ObjectTrackerLibraryDelegate> delegate;
@property (nonatomic, assign) bool recordDebugInfo;

+ (id)instance;

- (UIImage*)objectImage;
- (UIImage*)objectHistogram;
- (void)setObjectImageWithImage:(UIImage *)objectImage;
- (void)setObjectImageWithBuffer:(CVPixelBufferRef)objectImage;

- (void)trackObjectInImageWithImage:(UIImage *)image;
- (void)trackObjectInVideoWithImage:(UIImage *)image;
- (void)trackObjectInVideoWithBuffer:(CVPixelBufferRef)buffer;
- (Homography)homography;
- (BOOL)foundObject;

- (NSString*)frameDebugInfoString;
- (NSString*)videoDebugInfoString;
- (void)clearVideoDebugInfo;

- (UIImage*)detectionDebugImageWithSearchWindow:(BOOL)searchWindow;
- (UIImage*)validationDebugImageWithObjectRect:(BOOL)objectRect ObjectKeyPoints:(BOOL)objectKeyPoints SceneKeyPoints:(BOOL)sceneKeyPoints FilteredMatches:(BOOL)filteredMatches AllMatches:(BOOL)allmatches;
- (UIImage*)trackingDebugImageWithObjectRect:(BOOL)objectRect FilteredPoints:(BOOL)filteredPoints AllPoints:(BOOL)allPoints SearchWindow:(BOOL)searchWindow;

@end
