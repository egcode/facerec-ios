//
//  draw.cpp
//  opencvios
//
//  Created by Eugene G on 8/17/20.
//  Copyright © 2020 Eugene Golovanov. All rights reserved.
//

#ifndef draw_hpp
#define draw_hpp

#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


static cv::Mat drawRectsAndPoints(const cv::Mat &img, std::vector<Face> faces) {
  cv::Mat outImg;
  img.convertTo(outImg, CV_8UC3);
  float rect_thickness = 2;

  for (size_t i = 0; i < faces.size(); ++i) {
    auto rect = faces[i].bbox.getRect();
    // auto rect = faces[i].bbox.getSquare().getRect(); // Make Square

    float font_scale = 0.6;
    auto font = cv::FONT_HERSHEY_SIMPLEX;

//    double treshold = 0.6;
    double treshold = 0.5;

    if (faces[i].dist < treshold)
    {
      int baseline=0;
      cv::Size fontSize = cv::getTextSize(faces[i].label, font, font_scale, 2, &baseline);
      cv::Point p = cv::Point(rect.x,rect.y+rect.height+fontSize.height);

      // Text BG
      auto textBGRect = cv::Rect(p.x, rect.y+rect.height, fontSize.width, fontSize.height + (9*font_scale));
      cv::rectangle(outImg, textBGRect, cv::Scalar(0, 0, 0), cv::FILLED);

      // Text
      cv::putText(outImg, faces[i].label, p, font, font_scale, cv::Scalar(0, 255, 255), 2);

      // Main Face Recangle
      cv::rectangle(outImg, rect, cv::Scalar(0, 255, 255), rect_thickness);

    }
    else
    {
      int baseline=0;
      cv::Size fontSize = cv::getTextSize("unknown", font, font_scale, 2, &baseline);
      cv::Point p = cv::Point(rect.x,rect.y+rect.height+fontSize.height);

      // Text BG
      auto textBGRect = cv::Rect(p.x, rect.y+rect.height, fontSize.width, fontSize.height + (9*font_scale));
      cv::rectangle(outImg, textBGRect, cv::Scalar(0, 0, 0), cv::FILLED);

      // Text
      cv::putText(outImg, "unknown", p, font, font_scale, cv::Scalar(0, 0, 255), 2);
      
      // Main Face Recangle
      cv::rectangle(outImg, rect, cv::Scalar(0, 0, 255), rect_thickness);

    }


  }
  return outImg;
}

#endif
