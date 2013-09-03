#include "EyeTracker.h"

EyeTracker::EyeTracker(QObject *parent) : QObject(parent) {
    if (!faceCascade.load(kFaceCascadePath) || !eyesCascade.load(kEyesCascadePath)) {
        emit log("Cannot load cascade files...");
        exit(EXIT_FAILURE);
    }
    tracking = true;
}

EyeTracker::~EyeTracker() {
    stopTracking();
}

void EyeTracker::start() {
    cv::VideoCapture capture(-1);

    if (!capture.isOpened()) {
        std::cout << "failed to capture frames from webcam." << std::endl;
        return;
    }

    while (tracking) {
        cv::Mat frame;
        capture >> frame;
        if (!frame.empty()) {
            cv::cvtColor(frame, frame, CV_BGR2GRAY);
            cv::flip(frame, frame, 1);
            detectAndDisplay(frame);

            cv::cvtColor(frame, frame, CV_GRAY2RGB);
            emit frameReady(QImage(frame.data, frame.size().width, frame.size().height, frame.step, QImage::Format_RGB888));
        }
        msleep(1000/kFPS);
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
