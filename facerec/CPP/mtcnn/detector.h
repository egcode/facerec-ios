#ifndef detector_h
#define detector_h

#include "face.h"
#include "onet.h"
#include "pnet.h"
#include "rnet.h"

class MTCNNDetector {
private:
  std::unique_ptr<ProposalNetwork> _pnet;
  std::unique_ptr<RefineNetwork> _rnet;
  std::unique_ptr<OutputNetwork> _onet;

public:
  MTCNNDetector();

  MTCNNDetector(const ProposalNetwork::Config &pConfig,
                const RefineNetwork::Config &rConfig,
                const OutputNetwork::Config &oConfig);
  std::vector<Face> detect(const cv::Mat &img, const float minFaceSize,
                           const float scaleFactor);
};

#endif