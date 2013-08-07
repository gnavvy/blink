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

    void Start();

private:
    const std::string faceCascadePath = "../../../res/haarcascade_frontalface_alt.xml";
    const std::string eyesCascadePath = "../../../res/haarcascade_eye_tree_eyeglasses.xml";
    cv::CascadeClassifier faceCascade;
    cv::CascadeClassifier eyesCascade;
    cv::Mat debugImage_;
    void findEyes(cv::Mat frameGray, cv::Rect face);
    void detectAndDisplay(cv::Mat &frame);
    cv::Point findEyeCenter(cv::Mat face, cv::Rect eye, std::string debugWindow);

    void testPossibleCentersFormula(int x, int y, unsigned char weight, double gx, double gy, cv::Mat &out);


};

#endif // EYETRACKER_H
