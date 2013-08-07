#include "EyeTracker.h"

EyeTracker::EyeTracker() {}

void EyeTracker::Start() {
    if (!faceCascade.load(faceCascadePath) || !eyesCascade.load(eyesCascadePath)) {
        std::cerr << "cannot load cascade files" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string main_window_name = "Capture - Face detection";
    cv::namedWindow(main_window_name,CV_WINDOW_NORMAL);
    cv::moveWindow(main_window_name, 400, 100);
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
            cv::imshow(main_window_name, frame);

            if (static_cast<char>(cv::waitKey(10)) == 'c') {
                break;
            }
        }
    }
}

void EyeTracker::detectAndDisplay(cv::Mat &frame) {
    std::vector<cv::Rect> faces;
    cv::cvtColor(frame, frame, CV_BGR2GRAY);

    // detect faces
    faceCascade.detectMultiScale(frame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(150, 150));

    for (unsigned int i = 0; i < faces.size(); i++) {
        cv::rectangle(frame, faces[i], 1234);
        findEyes(frame, faces[i]);
    }
}

void EyeTracker::findEyes(cv::Mat frameGray, cv::Rect face) {
    cv::Mat faceROI = frameGray(face);
    cv::Mat debugFace = faceROI;

    if (kSmoothFaceImage) {
        double sigma = kSmoothFaceFactor * face.width;
        cv::GaussianBlur( faceROI, faceROI, cv::Size( 0, 0 ), sigma);
    }

    // find eye regions and draw them
    int eyeRegionWidth = face.width * kEyePercentWidth;
    int eyeRegionHeight = face.width * kEyePercentHeight;
    int eyeRegionTop = face.height * kEyePercentTop;
    int eyeRegionLeft = face.width * kEyePercentSide;
    int eyeRegionRight = face.width - eyeRegionWidth - eyeRegionLeft;

    cv::Rect leftEyeRegion(eyeRegionLeft, eyeRegionTop, eyeRegionWidth, eyeRegionHeight);
    cv::Rect rightEyeRegion(eyeRegionRight, eyeRegionTop, eyeRegionWidth, eyeRegionHeight);

    // find eye centers
    cv::Point leftPupil = findEyeCenter(faceROI, leftEyeRegion, "Left Eye");
    cv::Point rightPupil = findEyeCenter(faceROI, rightEyeRegion, "Right Eye");

    // find corner regions
    cv::Rect leftLeftCornerRegion(leftEyeRegion); {
        leftLeftCornerRegion.width = leftPupil.x;
        leftLeftCornerRegion.height /= 2;
        leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
    }
    cv::Rect leftRightCornerRegion(leftEyeRegion); {
        leftRightCornerRegion.width -= leftPupil.x;
        leftRightCornerRegion.height /= 2;
        leftRightCornerRegion.x += leftPupil.x;
        leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
    }
    cv::Rect rightLeftCornerRegion(rightEyeRegion); {
        rightLeftCornerRegion.width = rightPupil.x;
        rightLeftCornerRegion.height /= 2;
        rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
    }
    cv::Rect rightRightCornerRegion(rightEyeRegion); {
        rightRightCornerRegion.width -= rightPupil.x;
        rightRightCornerRegion.height /= 2;
        rightRightCornerRegion.x += rightPupil.x;
        rightRightCornerRegion.y += rightRightCornerRegion.height / 2;
    }

    cv::rectangle(debugFace, leftRightCornerRegion, 200);
    cv::rectangle(debugFace, leftLeftCornerRegion, 200);
    cv::rectangle(debugFace, rightLeftCornerRegion, 200);
    cv::rectangle(debugFace, rightRightCornerRegion, 200);

    // change eye centers to face coordinates
    leftPupil.x += leftEyeRegion.x;
    leftPupil.y += leftEyeRegion.y;
    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;

    // draw eye centers
    cv::circle(debugFace, leftPupil, 3, 1234);
    cv::circle(debugFace, rightPupil, 3, 1234);
}

cv::Point EyeTracker::findEyeCenter(cv::Mat face, cv::Rect eye, std::string debugWindow) {
    cv::Mat eyeRoIunscaled = face(eye);
    cv::Mat eyeROI;
    scaleToFastSize(eyeRoIunscaled, eyeROI);
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
    cv::imshow(debugWindow,gradientX);
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
            cv::imwrite("eyeFrame.png", eyeRoIunscaled);
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
