//
//   One-stop header.cpp
//  opencvios
//
//  Created by Eugene G on 8/29/20.
//  Copyright © 2020 Eugene Golovanov. All rights reserved.
//

#ifndef _include_dataset_face_h_
#define _include_dataset_face_h_

#include <iostream>
#include <torch/script.h> // One-stop header.


class DatasetFace {
      std::string name;
      at::Tensor embeddingTensor;
    public:
      DatasetFace(std::string n, at::Tensor et)
      {
        name = n;
        embeddingTensor = et;
      }
      std::string getName()
      {
        return name;
      }
      at::Tensor getEmbeddingTensor()
      {
        return embeddingTensor;
      }
};

#endif
