#ifndef EYETRACKER_H
#define EYETRACKER_H

#include <chrono>
#include <string>
#include <iostream>
#include <sstream>
#include <QDebug>
#include <QObject>
#include <QImage>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

class EyeTracker : public QObject {
    Q_OBJECT
public:
    explicit EyeTracker(QObject *parent = 0);
    virtual ~EyeTracker();
    void stopTracking() { tracking = false; }
    void pause() { pausing = true; }
    void resume() { pausing = false; }
    bool isPaused() const { return pausing; }

public slots:
    void start();

signals:
    void frameReady(QImage img);
    void blinkDetected();
    void finished();
    void log(QString msg);

private:
    const int kMinFace = 300;
    const int kMinEyes = 100;
    const int kFPS = 30;
    const cv::Size kVideoLogSize = cv::Size(320, 240);
    const std::string kWindowTitle = "Capture - Face detection";
    const std::string kFaceCascadePath = "./res/haarcascade_frontalface_alt.xml";
    const std::string kEyesCascadePath = "./res/haarcascade_eye_tree_eyeglasses.xml";

    cv::CascadeClassifier faceCascade;
    cv::CascadeClassifier eyesCascade;

    int numEyesCurr = 0;
    int numEyesPrev = 0;
    int numEyesHist = 0;

    bool tracking = false;
    bool pausing = false;

    void msleep(int ms);
    void detectAndDisplay(cv::Mat& frame);
};

#endif // EYETRACKER_H
