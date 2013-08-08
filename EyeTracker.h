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

class EyeTracker {
public:
    EyeTracker();
    void Start();

private:
    const std::string kWindowTitle = "Capture - Face detection";
    const std::string kFaceCascadePath = "../../../res/haarcascade_frontalface_alt.xml";
    const std::string kEyesCascadePath = "../../../res/haarcascade_eye_tree_eyeglasses.xml";

    // Size constants
    const float kEyeTop     = .32;
    const float kEyeSide    = .18;
    const float kEyeHeight  = .15;
    const float kEyeWidth   = .27;
    const float kMinFace    = 300;
    const float kMinEyes    = 100;

    const bool kUseCascade = false;

    const unsigned int kEyeBufferSize = 10;
    const int kFastEyeWidth = 50;
    const int kWeightBlurSize = 5;
    const bool kEnableWeight = true;
    const float kWeightDivisor = 150.0;
    const double kGradientThreshold = 50.0;

    cv::CascadeClassifier faceCascade, eyesCascade;
    cv::Mat debugImage_;
    cv::Mat sobel, sobelx, sobely;
    std::deque<cv::Point> eyeLs_, eyeRs_;

    // ------------------------ //

    cv::Point smoothedCenter();
    cv::Point findEyeCenter(cv::Mat& eyeROI, std::string debugWindow);
    void findEyes(cv::Mat& faceROI, const cv::Rect& faceRegion);
    void detectAndDisplay(cv::Mat& frame);
    void testPossibleCenters(int x, int y, unsigned char weight, double gx, double gy, cv::Mat &out);

    cv::Mat matrixMagnitude(const cv::Mat &matX, const cv::Mat &matY);
    cv::Mat computeMatXGradient(const cv::Mat &mat);
    double computeDynamicThreshold(const cv::Mat &mat, double stdDevFactor);
};

#endif // EYETRACKER_H
