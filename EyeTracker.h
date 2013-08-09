#ifndef EYETRACKER_H
#define EYETRACKER_H

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
    const int kMinFace = 300;
    const int kMinEyes = 100;
    const std::string kWindowTitle = "Capture - Face detection";
    const std::string kFaceCascadePath = "../../../res/haarcascade_frontalface_alt.xml";
    const std::string kEyesCascadePath = "../../../res/haarcascade_eye_tree_eyeglasses.xml";

    cv::CascadeClassifier faceCascade, eyesCascade;
    int numEyesCurr = 0;
    int numEyesPrev = 0;
    int numEyesHist = 0;

    void detectAndDisplay(cv::Mat& frame);
};

#endif // EYETRACKER_H
