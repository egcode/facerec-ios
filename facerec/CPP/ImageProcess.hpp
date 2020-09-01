//
//  ImageProcess.hpp
//  facerec
//
//  Created by Eugene G on 9/1/20.
//  Copyright © 2020 Eugene G. All rights reserved.
//

#ifndef ImageProcess_hpp
#define ImageProcess_hpp

#include <stdio.h>
#include <string>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "detector.h"
#include "draw.hpp"
#include "../CPP/dataset_face/DatasetFace.cpp"

class ImageProcess {
    private:
        MTCNNDetector detector;
        std::string mtcnnPath;
    
        torch::jit::script::Module module;
        std::vector<DatasetFace> datasetFaces;

    public:
        ImageProcess(std::string facerecPath, std::string mtcnnPath);
    
        // Facerec and MTCNN inferencing
        cv::Mat inference(cv::Mat src);
    
        void setDatasetFace(std::vector<DatasetFace> newDatasetFaces)
        {
            this->datasetFaces = newDatasetFaces;
        }

};

#endif
