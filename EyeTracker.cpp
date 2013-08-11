#include "EyeTracker.h"

EyeTracker::EyeTracker(QObject *parent) : QObject(parent) {
    capture_ = cvCaptureFromCAM(-1);

    if (!faceCascade.load(kFaceCascadePath) || !eyesCascade.load(kEyesCascadePath)) {
        emit log("Cannot load cascade files...");
        exit(EXIT_FAILURE);
    }
}

EyeTracker::~EyeTracker() {
    delete capture_;
}

void EyeTracker::Start() {
    std::cout << "capturing" << std::endl;
    if (capture_) {
        while (true) {
            frame_ = cvQueryFrame(capture_);
            if (frame_.empty()) {
                emit log(QString("no captured frame."));
                continue;
            }

            cv::cvtColor(frame_, frame_, CV_BGR2GRAY);
            cv::flip(frame_, frame_, 1);
            detectAndDisplay(frame_);

//            QImage img = QImage((uchar*)(frame_.data), frame_.cols, frame_.rows, QImage::Format_Indexed8);
//            emit processedImage(img);
        }
    }
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
