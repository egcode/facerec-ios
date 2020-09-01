//
//  recognition.hpp
//  opencvios
//
//  Created by Eugene G on 8/17/20.
//  Copyright © 2020 Eugene Golovanov. All rights reserved.
//

#ifndef recognition_hpp
#define recognition_hpp

#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <torch/script.h> // One-stop header.

#include "detector.h"

#include <string>
#include <vector>
#include <iostream>

#include "../dataset_face/DatasetFace.cpp"

double distanceCosine(at::Tensor tensor1, at::Tensor tensor2)
{
    /*
        ###### Based on `scipy`
        uv = np.average(embeddings1 * embeddings2)
        uu = np.average(np.square(embeddings1))
        vv = np.average(np.square(embeddings2))
        dist = 1.0 - uv / np.sqrt(uu * vv)
    */
    at::Tensor mult = tensor1 * tensor2;
    at::Tensor uv = mult.mean();

    at::Tensor arr1TensorSquare = tensor1 * tensor1;
    at::Tensor uu = arr1TensorSquare.mean();

    at::Tensor arr2TensorSquare = tensor2 * tensor2;
    at::Tensor vv = arr2TensorSquare.mean();

    at::Tensor uuMultVv = uu * vv;
    at::Tensor uuvvSqrt = uuMultVv.sqrt();

    at::Tensor distTensor = 1.0 - uv / uuvvSqrt;

    // convert tensor to double
    double* floatBuffer = distTensor.data_ptr<double>();
    double dist = floatBuffer[0];
    return dist;
}

at::Tensor emptyTensor()
{
    return torch::empty({1, 512}, torch::TensorOptions().dtype(torch::kFloat32));
}

cv::Mat cropFaceImage(Face face, cv::Mat img)
{

    // std::cout << "Img Size: " << img.size() << std::endl;

    // std::cout << "Before x1: " << face.bbox.x1 << std::endl;
    // std::cout << "Before x2: " << face.bbox.x2 << std::endl;
    // std::cout << "Before y1: " << face.bbox.y1 << std::endl;
    // std::cout << "Before y2: " << face.bbox.y2 << std::endl;

    //Border fix
    BBox newBBox = face.bbox;
    if (newBBox.x1 < 0) {
        newBBox.x1 = 0;
    }
    if (newBBox.x2 > img.size().width) {
        newBBox.x2 = img.size().width;
    }
    if (newBBox.y1 < 0) {
        newBBox.y1 = 0;
    }
    if (newBBox.y2 > img.size().height) {
        newBBox.y2 = img.size().height;
    }

// // cv::Rect rectSquare = face.bbox.getSquare().getRect(); // Make Square
    cv::Rect rectSquare = newBBox.getRect();
    
//     std::cout << "rect square: " << rectSquare << std::endl;

//     std::cout << "After x1: " << rectSquare.x << std::endl;
//     std::cout << "After x2: " << rectSquare.x + rectSquare.width << std::endl;
//     std::cout << "After y1: " << rectSquare.y << std::endl;
//     std::cout << "After y2: " << rectSquare.y + rectSquare.width << std::endl;
//     std::cout << "\n\n" << rectSquare.height << std::endl;

  
    // Crop the full image to that image contained by the rectangle myROI
    // Note that this doesn't copy the data
    cv::Mat croppedImage = img(rectSquare);

    cv::Mat reshapedImage;
    cv::Size size(112, 112);
    cv::resize(croppedImage,reshapedImage,size);//resize(src,dst,size)

    cv::Mat resultImage;
    reshapedImage.convertTo(resultImage, CV_32FC3, 1.0f / 255.0f);
    // std::cout << "resultImage: " << resultImage.size() << '\n';
    return resultImage;
}

torch::jit::script::Module torchInitModule(std::string modulePath)
{

  torch::jit::script::Module module;
  try {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    module = torch::jit::load(modulePath);
  }
  catch (const c10::Error& e) {
    std::cerr << "error loading face recognition model\n";
    // return -1;
      throw "error loading face recognition model\n";
  }
  std::cout << "face recognition model OK\n";
  return module;
}

at::Tensor torchFaceRecognitionInference(torch::jit::script::Module module, cv::Mat faceImage)
{
  // Get Tensor from Mat and shuffle channels
  at::Tensor tensorFromMat = torch::from_blob(faceImage.data, {1, 112, 112, 3});
  torch::jit::IValue input_tensor = tensorFromMat.permute({0, 3, 1, 2});

  // Add Tensor to actual inputs
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(input_tensor);

    // Disable back propagation
    torch::autograd::AutoGradMode guard(false);
    at::AutoNonVariableTypeMode non_var_type_mode(true);

    
  // --- Model Inference
  // Execute the model and turn its output into a tensor.
  at::Tensor output = module.forward(inputs).toTensor();
  //   std::cout << "Output slice 0-5: " << output.slice(/*dim=*/1, /*start=*/0, /*end=*/5) << '\n';
    // std::cout << "Output size: " << output.sizes() << '\n';
  return output;
}

std::vector<Face> readDatasetFacesAndGetLabels(std::vector<DatasetFace> datasetFaces, std::vector<Face> faces)
{
//    at::Tensor empTensor = emptyTensor();

    for (unsigned int i=0; i<datasetFaces.size();i++ )
    {

        for (unsigned int j=0; j<faces.size();j++ )
 
        {
            // Distance
            std::cout << "\n\nDistance Start------------------------------------------------: " << std::endl;

//            // Check if tensor is not empty
//            if (torch::equal(empTensor, faces[j].recognitionTensor) == 0)
//            {
            double dist = distanceCosine(faces[j].recognitionTensor, datasetFaces[i].getEmbeddingTensor());
            std::cout << "-----Comparing with : " << datasetFaces[i].getName() << "  Old name: " << faces[j].label << "   Old Distance: " << faces[j].dist << "\tNew Distance: " << dist << "\n"; // 1.0
            std::cout << "i:" << i << "   j:" << j << "\n";

            if (faces[j].dist > dist)
            {

                std::cout << "++++ REASSIGN To: " << datasetFaces[i].getName() << "\n";
                std::cout << "Old Distance: " << faces[j].dist << "\tNew Distance: " << dist << "\n";

                faces[j].dist = dist;
                faces[j].label = datasetFaces[i].getName();
                // std::cout << "Name : " << faces[j].label << " Distance : " << faces[j].dist << '\n'; // 1.0

            }
            std::cout << "\n\nDistance End--------------------------------------------------: " << std::endl;

//            } else {
//            std::cout << "\n\n\tArray Empty Tensor, ignoring..." << std::endl;
//            }

        }

    }
    return faces;
}


#endif
