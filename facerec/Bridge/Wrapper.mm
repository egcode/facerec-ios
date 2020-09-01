//
//  Wrapper.m
//  facerec
//
//  Created by Eugene G on 9/1/20.
//  Copyright © 2020 Eugene G. All rights reserved.
//

#import <opencv2/opencv.hpp>
#include "ImageProcess.hpp"
#import "Wrapper.h"
#import <UIKit/UIKit.h>

#include <string>
#import "FaceDataset.pbobjc.h"
#include"../CPP/dataset_face/DatasetFace.cpp"

@implementation Wrapper {
    ImageProcess *imageProcess;
}
using namespace std;

- (instancetype)initWithPaths:(NSString*)facerecPath datasetPath:(NSString*)datasetPath
{
    self = [super init];
    if (self) {
        self->imageProcess = new ImageProcess([facerecPath cStringUsingEncoding:NSUTF8StringEncoding],
                                              [[self getMTCNNPath] cStringUsingEncoding:NSUTF8StringEncoding]);
        self->imageProcess->setDatasetFace([self readDatasetFacesFromProtobuf:datasetPath]);
    }
    return self;
}

- (void)dealloc
{
    if (self->imageProcess) {
        delete self->imageProcess;
    }
}

- (UIImage *) processImage:(UIImage *)image {
    cv::Mat src=[self cvMatFromUIImage:image];
    cv::Mat processedImage;
    processedImage = self->imageProcess->inference(src);
    return [self UIImageFromCVMat:processedImage];
}

# pragma mark - MTCNN

- (NSString *)getMTCNNPath {
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *path = [[NSBundle mainBundle] pathForResource:@"det1" ofType:@"prototxt"];
    
    if ([fileManager fileExistsAtPath:path]){
        cout<<"exist";
        NSString *mainPath = [path substringToIndex:[path length]-13];
        return mainPath;

    } else {
        cout<<"not exist";
    }
    
    return @"";
}


# pragma mark - Protobuf

- (std::vector<DatasetFace>)readDatasetFacesFromProtobuf:(NSString*)datasetPath {

    std::vector<DatasetFace> datasetFaces;

    NSError* readFileError = nil;
    NSData* data = [NSData dataWithContentsOfFile:datasetPath  options:0 error:&readFileError];
    NSAssert((readFileError == nil), @"Data read from %@ with error: %@", datasetPath, readFileError);


    NSError* errorParse = nil;
    DatasetObject *datasetObject = [DatasetObject parseFromData:data error:&errorParse];
    NSAssert((errorParse == nil), @"Protobuf parse with error: %@", errorParse);

    for (int i = 0; i < datasetObject.faceobjectsArray_Count; i++) {
        FaceObject *faceObject = datasetObject.faceobjectsArray[i];

        NSLog(@"Name%d:  %@", i, faceObject.name);

        std::string nameCpp = [faceObject.name cStringUsingEncoding:NSUTF8StringEncoding];

        double embedding[512]; // 512

        // getting data from protobuf
        for (int j = 0; j < faceObject.embeddingsArray_Count; j++)
        {
            double d = [faceObject.embeddingsArray valueAtIndex:j];
            embedding[j] = d;
        }

        // Array to Tensor conversion V2 (copy memory)
        auto embTensor = torch::zeros( {1, 512},torch::kF64);
        std::memcpy(embTensor.data_ptr(),embedding,sizeof(double)*embTensor.numel());
        std::cout << "\tTensor Output slice 0-5: " << embTensor.slice(/*dim=*/1, /*start=*/0, /*end=*/5) << std::endl;

        // Adding to dataset;
        DatasetFace df = DatasetFace(nameCpp, embTensor);
        datasetFaces.push_back(df);

    }


    return datasetFaces;
}

# pragma mark - Global Stuff

- (cv::Mat)cvMatFromUIImage:(UIImage *)image
{
    CGColorSpaceRef colorSpace = CGImageGetColorSpace(image.CGImage);
    CGFloat cols = image.size.width;
    CGFloat rows = image.size.height;
    
    cv::Mat cvMat(rows, cols, CV_8UC4); // 8 bits per component, 4 channels (color channels + alpha)
    
    CGContextRef contextRef = CGBitmapContextCreate(cvMat.data,                 // Pointer to  data
                                                    cols,                       // Width of bitmap
                                                    rows,                       // Height of bitmap
                                                    8,                          // Bits per component
                                                    cvMat.step[0],              // Bytes per row
                                                    colorSpace,                 // Colorspace
                                                    kCGImageAlphaNoneSkipLast |
                                                    kCGBitmapByteOrderDefault); // Bitmap info flags
    
    CGContextDrawImage(contextRef, CGRectMake(0, 0, cols, rows), image.CGImage);
    CGContextRelease(contextRef);
    
    return cvMat;
}

-(UIImage *)UIImageFromCVMat:(cv::Mat)cvMat
{
    NSData *data = [NSData dataWithBytes:cvMat.data length:cvMat.elemSize()*cvMat.total()];
    CGColorSpaceRef colorSpace;
    
    if (cvMat.elemSize() == 1) {
        colorSpace = CGColorSpaceCreateDeviceGray();
    } else {
        colorSpace = CGColorSpaceCreateDeviceRGB();
    }
    
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
    
    // Creating CGImage from cv::Mat
    CGImageRef imageRef = CGImageCreate(cvMat.cols,                                 //width
                                        cvMat.rows,                                 //height
                                        8,                                          //bits per component
                                        8 * cvMat.elemSize(),                       //bits per pixel
                                        cvMat.step[0],                            //bytesPerRow
                                        colorSpace,                                 //colorspace
                                        kCGImageAlphaNone|kCGBitmapByteOrderDefault,// bitmap info
                                        provider,                                   //CGDataProviderRef
                                        NULL,                                       //decode
                                        false,                                      //should interpolate
                                        kCGRenderingIntentDefault                   //intent
                                        );
    
    // Getting UIImage from CGImage
    UIImage *finalImage = [UIImage imageWithCGImage:imageRef];
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    
    return finalImage;
}


@end

