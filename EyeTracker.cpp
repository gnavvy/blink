#include "EyeTracker.h"

EyeTracker::EyeTracker() {}

void EyeTracker::Start() {
    if (!faceCascade.load(faceCascadePath)) {
        std::cerr << "cannot load face cascade files" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!leftEyeCascade.load(leftEyeCascadePath) || !rightEyeCascase.load(rightEyeCascadePath)) {
        std::cerr << "cannot load eyes cascade files" << std::endl;
        exit(EXIT_FAILURE);
    }

    cv::namedWindow(kWindowTitle,CV_WINDOW_NORMAL);
    cv::moveWindow(kWindowTitle, 400, 100);
    cv::namedWindow("Right Eye",CV_WINDOW_NORMAL);
    cv::moveWindow("Right Eye", 10, 600);
    cv::namedWindow("Left Eye",CV_WINDOW_NORMAL);
    cv::moveWindow("Left Eye", 10, 800);

    CvCapture *capture = cvCaptureFromCAM(-1);
    cv::Mat frame;
    if (capture) {
        while (true) {
            frame = cvQueryFrame(capture);
            if (frame.empty()) {
                std::cerr << "no captured frame." << std::endl; break;
            }

            cv::flip(frame, frame, 1);  // mirror
            detectAndDisplay(frame);

            cv::imshow(kWindowTitle, frame);
            if (static_cast<char>(cv::waitKey(10)) == 'c') {
                break;
            }
        }
    }
}

void EyeTracker::detectAndDisplay(cv::Mat& frame) {
    std::vector<cv::Rect> faces;
    cv::cvtColor(frame, frame, CV_BGR2GRAY);
    cv::equalizeHist(frame, frame);

    // detect faces
    faceCascade.detectMultiScale(frame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(200, 200));

    for (unsigned int i = 0; i < faces.size(); i++) {
        cv::Rect faceRegion = faces[i];
        std::vector<cv::Rect> eyesRegion = estimateEyesRegion(faceRegion);

        cv::Mat faceROI = frame(faceRegion);
//        cv::Mat eyesROI = faceROI(eyesRegion);

        if (kUseCascade) {
            std::vector<cv::Rect> leftEyes, rightEyes;
            leftEyeCascade.detectMultiScale(faceROI, leftEyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(50, 50));
            rightEyeCascase.detectMultiScale(faceROI, rightEyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(50, 50));

            for (unsigned int j = 0; j < leftEyes.size(); j++) {
                cv::Point center(faceRegion.x + leftEyes[j].x + leftEyes[j].width*0.5,
                                 faceRegion.y + leftEyes[j].y + leftEyes[j].height*0.5);
                int radius = cvRound((leftEyes[j].width + leftEyes[i].height)*0.25);
                cv::circle(frame, center, radius, cv::Scalar(255, 0, 0), 2, 2, 0);
            }

            for (unsigned int j = 0; j < rightEyes.size(); j++) {
                cv::Point center(faceRegion.x + rightEyes[j].x + rightEyes[j].width*0.5,
                                 faceRegion.y + rightEyes[j].y + rightEyes[j].height*0.5);
                int radius = cvRound((rightEyes[j].width + rightEyes[i].height)*0.25);
                cv::circle(frame, center, radius, cv::Scalar(0, 0, 255), 2, 2, 0);
            }
        } else {    // use gradient
            findEyes(faceROI, faceRegion);
        }
    }
}

std::vector<cv::Rect> EyeTracker::estimateEyesRegion(const cv::Rect& faceRegion) {
    std::vector<cv::Rect> eyesRegion;

    int eyeRegionLeft   = faceRegion.width  * kEyeSide;
    int eyeRegionTop    = faceRegion.height * kEyeTop;
    int eyeRegionWidth  = faceRegion.width  * kEyeWidth;
    int eyeRegionHeight = faceRegion.height * kEyeHeight;
    int eyeRegionRight  = faceRegion.width  - eyeRegionWidth - eyeRegionLeft;

    cv::Rect leftEyeRegion(eyeRegionLeft, eyeRegionTop, eyeRegionWidth, eyeRegionHeight);
    cv::Rect rightEyeRegion(eyeRegionRight, eyeRegionTop, eyeRegionWidth, eyeRegionHeight);

    eyesRegion.push_back(leftEyeRegion);
    eyesRegion.push_back(rightEyeRegion);

    return eyesRegion;
}

void EyeTracker::findEyes(cv::Mat& faceROI, cv::Rect& faceRegion) {
    std::vector<cv::Rect> eyesRegions = estimateEyesRegion(faceRegion);

    // find eye centers
    cv::Point leftCenter = findEyeCenter(faceROI, eyesRegions[0], "Left Eye");
    cv::Point rightCenter = findEyeCenter(faceROI, eyesRegions[1], "Right Eye");

    // find corner regions
    cv::Rect leftLeftCornerRegion(eyesRegions[0]); {
        leftLeftCornerRegion.width = leftCenter.x;
        leftLeftCornerRegion.height /= 2;
        leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
    }
    cv::Rect leftRightCornerRegion(eyesRegions[0]); {
        leftRightCornerRegion.width -= leftCenter.x;
        leftRightCornerRegion.height /= 2;
        leftRightCornerRegion.x += leftCenter.x;
        leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
    }
    cv::Rect rightLeftCornerRegion(eyesRegions[1]); {
        rightLeftCornerRegion.width = rightCenter.x;
        rightLeftCornerRegion.height /= 2;
        rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
    }
    cv::Rect rightRightCornerRegion(eyesRegions[1]); {
        rightRightCornerRegion.width -= rightCenter.x;
        rightRightCornerRegion.height /= 2;
        rightRightCornerRegion.x += rightCenter.x;
        rightRightCornerRegion.y += rightRightCornerRegion.height / 2;
    }

    cv::rectangle(faceROI, leftRightCornerRegion, 200, 2, 2, 0);
    cv::rectangle(faceROI, leftLeftCornerRegion, 200, 2, 2, 0);
    cv::rectangle(faceROI, rightLeftCornerRegion, 200, 2, 2, 0);
    cv::rectangle(faceROI, rightRightCornerRegion, 200, 2, 2, 0);

    // change eye centers to face coordinates
    leftCenter.x += eyesRegions[0].x;
    leftCenter.y += eyesRegions[0].y;
    rightCenter.x += eyesRegions[1].x;
    rightCenter.y += eyesRegions[1].y;

    // draw eye centers
    cv::circle(faceROI, leftCenter, 3, 1234);
    cv::circle(faceROI, rightCenter, 3, 1234);
}

cv::Point EyeTracker::findEyeCenter(cv::Mat face, cv::Rect eye, std::string debugWindow) {
    cv::Mat eyeROIunscaled = face(eye);
    cv::Mat eyeROI;
    scaleToFastSize(eyeROIunscaled, eyeROI);
    // draw eye region
    cv::rectangle(face, eye, 1234);

    // find the gradient
    cv::Mat gradientX = computeMatXGradient(eyeROI);
    cv::Mat gradientY = computeMatXGradient(eyeROI.t()).t();
    // compute magnitudes
    cv::Mat magnitudes = matrixMagnitude(gradientX, gradientY);
    // compute threshold
    double gradientThreshold = computeDynamicThreshold(magnitudes, kGradientThreshold);
    // nomalize
    for (int y = 0; y < eyeROI.rows; y++) {
        double *xr = gradientX.ptr<double>(y);
        double *yr = gradientY.ptr<double>(y);
        double *mr = magnitudes.ptr<double>(y);
        for (int x = 0; x < eyeROI.cols; x++) {
            double gx = xr[x];
            double gy = yr[x];
            double mag = mr[x];
            xr[x] = mag > gradientThreshold ? gx / mag : 0.0;
            yr[x] = mag > gradientThreshold ? gy / mag : 0.0;
        }
    }

    cv::imshow(debugWindow, gradientX);

    // create a blurred and inverted image for weighting
    cv::Mat weight;
    cv::GaussianBlur( eyeROI, weight, cv::Size( kWeightBlurSize, kWeightBlurSize ), 0, 0 );       // ?
    for (int y = 0; y < weight.rows; y++) {
        unsigned char *row = weight.ptr<unsigned char>(y);
        for (int x = 0; x < weight.cols; x++) {
            row[x] = 255 - row[x];
        }
    }
    // run the algorithm
    cv::Mat outSum = cv::Mat::zeros(eyeROI.rows, eyeROI.cols, CV_64F);
    // for each possible center

    for (int y = 0; y < weight.rows; y++) {
        const unsigned char *wr = weight.ptr<unsigned char>(y);
        const double *xr = gradientX.ptr<double>(y);
        const double *yr = gradientY.ptr<double>(y);
        for (int x = 0; x < weight.cols; x++) {
            double gx = xr[x];
            double gy = yr[x];
            if (gx == 0.0 && gy == 0.0) continue;
            testPossibleCentersFormula(x, y, wr[x], gx, gy, outSum);
        }
    }

    // scale all the values down, basically averaging them
    double numGradients = weight.rows * weight.cols;
    cv::Mat out;
    outSum.convertTo(out, CV_32F, 1.0 / numGradients);
    // find the max point?
    cv::Point maxP;
    double maxVal;
    cv::minMaxLoc(out, nullptr, &maxVal, nullptr, &maxP);

    // flood fill the edges
    if (kEnablePostProcess) {
        cv::Mat floodClone;
        double floodThreshold = maxVal * kPostProcessThreshold;
        cv::threshold(out, floodClone, floodThreshold, 0.0f, cv::THRESH_TOZERO);
        if (kPlotVectorField) {
            cv::imwrite("eyeFrame.png", eyeROIunscaled);
        }
        cv::Mat mask = floodKillEdges(floodClone);
        cv::minMaxLoc(out, nullptr, &maxVal, nullptr, &maxP, mask);
    }

    return unscalePoint(maxP, eye);
}

void EyeTracker::testPossibleCentersFormula(int x, int y, unsigned char weight, double gx, double gy, cv::Mat &out) {
    // for all possible centers
    for (int cy = 0; cy < out.rows; cy++) {
        double *Or = out.ptr<double>(cy);
        for (int cx = 0; cx < out.cols; cx++) {
            if (x == cx && y == cy) continue;
            // create a vector from the possible center to the gradient origin
            double dx = x - cx;
            double dy = y - cy;
            // normalize d
            double magnitude = sqrt((dx * dx) + (dy * dy));
            dx = dx / magnitude;
            dy = dy / magnitude;
            double dotProduct = dx*gx + dy*gy;
            dotProduct = std::max(0.0,dotProduct);
            // square and multiply by the weight
            if (kEnableWeight) {
                Or[cx] += dotProduct * dotProduct * (weight/kWeightDivisor);
            } else {
                Or[cx] += dotProduct * dotProduct;
            }
        }
    }
}
