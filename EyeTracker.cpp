#include "EyeTracker.h"

EyeTracker::EyeTracker(QObject *parent) : QObject(parent) {
    if (!faceCascade.load(kFaceCascadePath) || !eyesCascade.load(kEyesCascadePath)) {
        emit log("Cannot load cascade files...");
        exit(EXIT_FAILURE);
    }
    tracking = true;
}

EyeTracker::~EyeTracker() {}

void EyeTracker::run() {
    capture_ = cvCaptureFromCAM(-1);  // use default camera
    if (capture_) {
        while (tracking) {
            frame_ = cvQueryFrame(capture_);
            if (frame_.empty()) {
                continue;
            }

            cv::cvtColor(frame_, frame_, CV_BGR2GRAY);
            cv::flip(frame_, frame_, 1);
            detectAndDisplay(frame_);

            msleep(30);
        }
        cvReleaseCapture(&capture_);
    }
    emit finished();
}

void EyeTracker::detectAndDisplay(cv::Mat& frame) {
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(frame, faces, 1.1, 2, CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(kMinFace, kMinFace));
    if (faces.empty()) { return; }

    cv::Rect faceRegion = faces[0];
    cv::Mat faceROI = frame(faceRegion);
    cv::rectangle(frame, faceRegion, 1234);

    std::vector<cv::Rect> eyeRegions;
    eyesCascade.detectMultiScale(faceROI, eyeRegions, 1.2, 2, CV_HAAR_SCALE_IMAGE, cv::Size(kMinEyes, kMinEyes));
    numEyesCurr = eyeRegions.size();
    if (numEyesCurr == 2) {
        numEyesHist++;
    } else if (numEyesCurr == 0 && numEyesHist > 10) {
        numEyesHist = 0;
        emit blinkDetected();
    }
    for (int j = 0; j < numEyesCurr; j++) {
        cv::Rect eyeRegion = eyeRegions[j];
        cv::rectangle(faceROI, eyeRegion, 1234);
    }
}

void EyeTracker::msleep(int ms) {
    struct timespec ts = {ms/1000, (ms%1000)*1000*1000};
    nanosleep(&ts, nullptr);
}
