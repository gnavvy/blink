// Publication: Timm and Barth. Accurate eye centre localisation by means of gradients.
// In Proceedings of the Int. Conference on Computer Theory and Applications (VISAPP)
// Volume 1, pages 125-130, Algarve, Portugal, 2011. INSTICC.
// Modified from eyeLike: https://github.com/trishume/eyeLike

#ifndef EYETRACKER_H
#define EYETRACKER_H

#include <ctime>
#include <array>
#include <deque>
#include <vector>
#include <iostream>
#include <QObject>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

class EyeTracker : public QObject {
    Q_OBJECT
public:
    explicit EyeTracker(QObject *parent = 0);
    void Start();

signals:
    void blinked();

private:
    const std::string kWindowTitle = "Capture - Face detection";
    const std::string kFaceCascadePath = "../../../res/haarcascade_frontalface_alt.xml";
    const std::string kEyesCascadePath = "../../../res/haarcascade_eye_tree_eyeglasses.xml";

    // Size constants
    const float kEyeLeft    = .20;
    const float kEyeTop     = .40;
    const float kEyeHeight  = .10;
    const float kEyeWidth   = .20;
    const float kMinFace    = 300;
    const float kMinEyes    = 100;

    const bool kUseCascade = true;

    const unsigned int kEyeBufferSize = 20;
    const int kHistRes = 256;
    const int kFastEyeWidth = 50;
    const int kWeightBlurSize = 5;
    const bool kEnableWeight = true;
    const float kWeightDivisor = 150.0;
    const double kGradientThreshold = 50.0;

    std::vector<int> kHistRange;

    cv::CascadeClassifier faceCascade, eyesCascade;
    cv::Mat debugImage_;
    cv::Mat eyeLHistHistory_, eyeLHistCurrent_;
    std::deque<cv::Point> eyeLs_, eyeRs_;
    std::deque<cv::Mat> eyeLROIs_;

    // ------------------------ //

    int numEyesCurr = 0;
    int numEyesPrev = 0;
    int numEyesHist = 0;

    bool isGazed = false;
    int numEyesOpen = 0;
    int numDetectedEyes = 0;

    cv::Point smoothedCenter();
    cv::Point findEyeCenter(cv::Mat& eyeROI);
    void findEyes(cv::Mat& faceROI, const cv::Rect& faceRegion);
    void detectAndDisplay(cv::Mat& frame);
    void testPossibleCenters(int x, int y, unsigned char weight, double gx, double gy, cv::Mat &out);

    cv::Mat matrixMagnitude(const cv::Mat &matX, const cv::Mat &matY);
    cv::Mat computeMatXGradient(const cv::Mat &mat);
    double computeDynamicThreshold(const cv::Mat &mat, double stdDevFactor);
};

#endif // EYETRACKER_H
