#include "EyeTracker.h"

EyeTracker::EyeTracker(QObject *parent) : QObject(parent) {}

void EyeTracker::Start() {
    if (!faceCascade.load(kFaceCascadePath)) {
        std::cerr << "cannot load face cascade files" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!eyesCascade.load(kEyesCascadePath)) {
        std::cerr << "cannot load eyes cascade files" << std::endl;
        exit(EXIT_FAILURE);
    }

    cv::namedWindow(kWindowTitle,CV_WINDOW_NORMAL);
    cv::moveWindow(kWindowTitle, 640, 100);

    CvCapture *capture = cvCaptureFromCAM(-1);
    cv::Mat frame;
    if (capture) {
        while (true) {
            frame = cvQueryFrame(capture);
            if (frame.empty()) { std::cerr << "no captured frame." << std::endl; break; }
            cv::flip(frame, frame, 1);  // mirror
            detectAndDisplay(frame);
            cv::imshow(kWindowTitle, frame);
            if (static_cast<char>(cv::waitKey(10)) == 27 /*esc*/) { exit(EXIT_SUCCESS); }
        }
    }
}

void EyeTracker::detectAndDisplay(cv::Mat& frame) {
    std::vector<cv::Rect> faces;
    cv::cvtColor(frame, frame, CV_BGR2GRAY);

    // detect faces
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
        emit blinked();
    }
    for (int j = 0; j < numEyesCurr; j++) {
        cv::Rect eyeRegion = eyeRegions[j];
        cv::rectangle(faceROI, eyeRegion, 1234);
    }
}
