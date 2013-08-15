#ifndef EYETRACKER_H
#define EYETRACKER_H

#include <iostream>
#include <QDebug>
#include <QObject>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "Utilities.h"

class EyeTracker : public QObject {
    Q_OBJECT
public:
    explicit EyeTracker(QObject *parent = 0);
    virtual ~EyeTracker();
    void StopTracking() { tracking = false; }

public slots:
    void Start() { run(); }

protected:
    void run();

signals:
    void blinkDetected();
    void finished();
    void log(QString msg);

private:
    const int kMinFace = 300;
    const int kMinEyes = 100;
    const std::string kWindowTitle = "Capture - Face detection";
    const std::string kFaceCascadePath = "../../../res/haarcascade_frontalface_alt.xml";
    const std::string kEyesCascadePath = "../../../res/haarcascade_eye_tree_eyeglasses.xml";

    cv::CascadeClassifier faceCascade;
    cv::CascadeClassifier eyesCascade;
    CvCapture *capture;
    cv::Mat frame;

    int numEyesCurr = 0;
    int numEyesPrev = 0;
    int numEyesHist = 0;

    bool tracking = false;

    void msleep(int ms);
    void detectAndDisplay(cv::Mat& frame);
};

#endif // EYETRACKER_H
