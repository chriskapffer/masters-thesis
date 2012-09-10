//
//  ObjectTrackerLibrary.h
//  CKObjectTrackerShared
//
//  Created by Christoph Kapffer on 07.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>

#define MAX_RECORDED_FRAMES 10000

typedef struct {
    CGFloat m11, m12, m13;
    CGFloat m21, m22, m23;
    CGFloat m31, m32, m33;
} Homography;

@protocol ObjectTrackerLibraryDelegate <NSObject>

@optional
- (void)trackedObjectWithHomography:(Homography)homography;
- (void)reachedDebugInfoRecordingLimit:(NSString*)debugInfo;
- (void)failedToTrackObjectInImage;
- (void)trackerLibraryDidProcessFrame;

@end

@class ObjectTrackerParameterCollection;

@interface ObjectTrackerLibrary : NSObject

@property (nonatomic, assign) bool recordDebugInfo;
@property (nonatomic, weak) id<ObjectTrackerLibraryDelegate> delegate;
@property (nonatomic, strong, readonly) ObjectTrackerParameterCollection* parameters;

@property (atomic, assign, readonly) Homography homography;
@property (atomic, assign, readonly) BOOL foundObject;
@property (atomic, copy, readonly) NSString* frameDebugInfoString;
@property (atomic, copy, readonly) NSString* videoDebugInfoString;

+ (id)instance;

- (UIImage*)objectImage;
- (UIImage*)objectHistogram;
- (void)setObjectImageWithImage:(UIImage *)objectImage;
- (void)setObjectImageWithBuffer:(CVPixelBufferRef)objectImage;

- (void)setBoolParameterWithName:(NSString*)name Value:(BOOL)value;
- (void)setintParameterWithName:(NSString*)name Value:(int)value;
- (void)setFloatParameterWithName:(NSString*)name Value:(float)value;
- (void)setStringParameterWithName:(NSString*)name Value:(NSString*)value;

- (void)trackObjectInImageWithImage:(UIImage *)image;
- (void)trackObjectInVideoWithImage:(UIImage *)image;
- (void)trackObjectInVideoWithBuffer:(CVPixelBufferRef)buffer;

- (void)clearVideoDebugInfo;

- (BOOL)detectionDebugImage:(UIImage**)image WithSearchWindow:(BOOL)searchWindow;
- (BOOL)validationDebugImage:(UIImage**)image WithObjectRect:(BOOL)objectRect ObjectKeyPoints:(BOOL)objectKeyPoints SceneKeyPoints:(BOOL)sceneKeyPoints FilteredMatches:(BOOL)filteredMatches AllMatches:(BOOL)allmatches;
- (BOOL)trackingDebugImage:(UIImage**)image WithObjectRect:(BOOL)objectRect FilteredPoints:(BOOL)filteredPoints AllPoints:(BOOL)allPoints SearchWindow:(BOOL)searchWindow;

@end
