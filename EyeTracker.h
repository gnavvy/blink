// Publication: Timm and Barth. Accurate eye centre localisation by means of gradients.
// In Proceedings of the Int. Conference on Computer Theory and Applications (VISAPP)
// Volume 1, pages 125-130, Algarve, Portugal, 2011. INSTICC.
// Modified from eyeLike: https://github.com/trishume/eyeLike

#ifndef EYETRACKER_H
#define EYETRACKER_H

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "helpers.h"
#include "constants.h"

#define kEyeLeft true;
#define kEyeRight false;

class EyeTracker {
public:
    EyeTracker();
    void DetectAndDisplay(cv::Mat frame);
    void Start();

private:
    const std::string face_cascade_name = "../../../res/haarcascade_frontalface_alt.xml";
    cv::CascadeClassifier face_cascade;
    cv::Mat skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);
    cv::Mat *leftCornerKernel;
    cv::Mat *rightCornerKernel;
    cv::Mat debugImage_;
    float kEyeCornerKernel[4][6] = {
        {-1,-1,-1, 1, 1, 1},
        {-1,-1,-1,-1, 1, 1},
        {-1,-1,-1,-1, 0, 3},
        { 1, 1, 1, 1, 1, 1},
    };

    void createCornerKernels();
    void releaseCornerKernels();
    void findEyes(cv::Mat frame_gray, cv::Rect face);
    cv::Point findEyeCenter(cv::Mat face, cv::Rect eye, std::string debugWindow);
    cv::Point2f findEyeCorner(cv::Mat region, bool left, bool left2);
    cv::Point2f findSubpixelEyeCorner(cv::Mat region, cv::Point maxP);
    cv::Mat eyeCornerMap(const cv::Mat &region, bool left, bool left2);

    void testPossibleCentersFormula(int x, int y, unsigned char weight, double gx, double gy, cv::Mat &out);


};

#endif // EYETRACKER_H
