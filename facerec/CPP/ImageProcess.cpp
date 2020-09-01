//
//  ImageProcess.cpp
//  facerec
//
//  Created by Eugene G on 9/1/20.
//  Copyright © 2020 Eugene G. All rights reserved.
//

#include "ImageProcess.hpp"
#include "mtcnn/recognition.hpp"

ImageProcess::ImageProcess(std::string facerecPath, std::string mtcnnPath)
{
    this->mtcnnPath = mtcnnPath;
//    std::cout<<"printing mtcnnPath: "<<mtcnnPath<<std::endl;
    
    ProposalNetwork::Config pConfig;
    pConfig.caffeModel = mtcnnPath + "det1.caffemodel";
    pConfig.protoText = mtcnnPath + "det1.prototxt";
    pConfig.threshold = 0.6f;
    
    RefineNetwork::Config rConfig;
    rConfig.caffeModel = mtcnnPath + "det2.caffemodel";
    rConfig.protoText = mtcnnPath + "det2.prototxt";
    rConfig.threshold = 0.7f;
    
    OutputNetwork::Config oConfig;
    oConfig.caffeModel = mtcnnPath + "det3.caffemodel";
    oConfig.protoText = mtcnnPath + "det3.prototxt";
    oConfig.threshold = 0.7f;
    

    this->detector = MTCNNDetector(pConfig, rConfig, oConfig);
    
    /////////// --- Recognition Init start
    std::cout.precision(17);
    this->module = torchInitModule(facerecPath);

    /////////// --- Recognition Init end

}


cv::Mat ImageProcess::inference(cv::Mat src)
{
    cv::Mat frame;
    cv::cvtColor(src, frame, cv::COLOR_BGR2RGB);

    if (this->mtcnnPath == "") {
        return frame;
    }
    
    std::vector<Face> faces;
    
    // Face Detection
    {
        faces = this->detector.detect(frame, 20.f, 0.709f);
    }
        
     std::cout << "Number of faces found in the supplied image - " << faces.size() << std::endl;

     // Face Recognition
     for (size_t i = 0; i < faces.size(); ++i)
     {
         cv::Mat faceImage = cropFaceImage(faces[i], frame);
         faces[i].recognitionTensor = torchFaceRecognitionInference(module, faceImage);
     }

     faces = readDatasetFacesAndGetLabels(this->datasetFaces, faces);

     // Show Result
     auto resultImg = drawRectsAndPoints(frame, faces);
    
     cv::Mat resultImgRGB;
     cv::cvtColor(resultImg, resultImgRGB, cv::COLOR_RGB2BGR);

    return resultImgRGB;
}
