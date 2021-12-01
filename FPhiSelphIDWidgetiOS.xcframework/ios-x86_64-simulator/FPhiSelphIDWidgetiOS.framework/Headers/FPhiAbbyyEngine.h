#ifndef FPhiAbbyyEngine_h
#define FPhiAbbyyEngine_h

#import <Foundation/Foundation.h>

#import "AbbyyRtrSDK/AbbyyRtrSDK.h"

#import "FPhiSelphIDDocumentManager.h"
#import "FPhiSelphIDError.h"

/// Further information https://help.abbyy.com/en-us/mobilecapturesdk/1/ios_help/introduction

#pragma mark - DocumenSide

/// Enum for the document side
typedef NS_ENUM(NSUInteger, DocumentSide)
{
    FRONT = 0,
    BACK  = 1,
};

#pragma mark - DocumentSize

//#define TD1 CGSizeMake(53.98f, 85.6f)
#define TD1 CGSizeMake(85.6f, 53.98f)

/// Document size used by ABBYY Engine
typedef CGSize DocumentSize;

#pragma mark - Protocols

/// To communicate with the camera (fill buffers with the image)
@protocol AbbyyCameraCallback <NSObject>

-(void)onFrameFilled:(CMSampleBufferRef)buffer withUIImage:(UIImage *)image;

@end

/// To obtain the result from ABBYY engine
@protocol AbbyyResultCallback <NSObject>

-(void)onResult:(NSMutableDictionary<NSString *, NSString *> *)data :(NSMutableDictionary<NSString *, UIImage *> *)croppedImages;
-(void)onRetry:(NSString *)cause;
-(void)onError:(FPhiSelphIDWidgetError)type :(NSString *)message;

@end

/// To implement by custom mappers and map the result from ABBYY to FPhiWidget
@protocol AbbyyMapperResult <NSObject>

-(NSMutableDictionary<NSString *, NSString *> *)mapper:(FPhiSelphIDDocumentManager *)manager key:(NSString *)key data:(NSArray<RTRDataField *> *)fields;

@end

#pragma mark - FPhiAbbyyEngine - Public declaration

@interface FPhiAbbyyEngine : NSObject<AbbyyCameraCallback, RTRDataCaptureServiceDelegate>
{
    /// Engine for AbbyyRtrSDK.
    RTREngine *_engine;

    /// To retrieve OCR from ABBYY capture service
    id<RTRDataCaptureService> _ocrCaptureService;

    /// To avoid receive more results from ABBYY when a stable OCR result has been reached
    BOOL _ocrResultHasBeenReached;
    BOOL _ocrResultProcessing;

    /// Mutext to synchronize the ocr result flag
    NSLock *_mutex;
    
    /// Buffer for storing frame image when using DataCapture Service
    UIImage *_imageCapturedByOCR;

    /// Width of the camera
    int _width;

    /// Height of the camera
    int _height;

    /// Image orientation
    UIImageOrientation _orientation;

    /// Region of interest where to find the data
    CGRect _roi;
    
    /// Key used to store the ABBYY result into the FPhiWidget result
    NSString *_key;

    /// Convert ABBYY result to FPhiWidget result
    id<AbbyyMapperResult> _mapper;

    /// Handler to manage tasks in background
    NSOperationQueue *_queue;
    
    // TODO: Delete
    FPhiSelphIDDocumentManager *_documentManager;
}

/// Selected profile: if none is chosen, ID_ES will be the default value.
@property(nonatomic) NSString *profile;

/// Document side: Front or Back
@property(nonatomic, readonly, assign) DocumentSide documentSide;

/// Document size for the cropped image
@property(nonatomic, assign) DocumentSize documentSize;

/// Document ID: Document's country ISO_alpha2
@property(nonatomic) NSString *documentID;

/// Document Version based on .xml
@property(nonatomic) NSString *documentVersion;

/// Callback with results from ABBYY engine
@property(nonatomic, weak) id<AbbyyResultCallback> resultCallback;

/// Constructor
-(id)initWithLicense:(NSString *)licensePath;

/// Starts the ABBYY engine.
-(void)start:(int)width height:(int)height orientation:(UIImageOrientation)orientation roi:(CGRect)roi;

/// Stops the ABBYY engine
-(void)stop;

/// The buffer filled by the camera. It will sent into the ABBYY services
-(id<AbbyyCameraCallback>)getCameraInputCallback;

/// Set a valid profile to use with ABBYY engine
-(void)setProfile:(NSString *)profile;

/// Set the document side to be scanned
-(void)setDocumentSide:(DocumentSide)side;

/// Set a valid document size to use with ABBYY engine
-(void)setDocumentSize:(DocumentSize)size;

/// Set the document's country in ISO alpha2 format
-(void)setDocumentCountry:(NSString *)country;

/// Set the document's version based on .xml model
-(void)setDocumentVersion:(NSString *)version;

/// Gets the document's version captured by the engine
-(NSString *)getDocumentVersionCaptured;

/// Callback to receive the ABBYY results
-(void)setResultCallback:(id<AbbyyResultCallback>)callback;

// TODO: Delete
-(void)setDocumentManager:(FPhiSelphIDDocumentManager *)manager;

@end

#endif
