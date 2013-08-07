#include "EyeTracker.h"

EyeTracker::EyeTracker() {}

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
    cv::namedWindow("Right Eye",CV_WINDOW_NORMAL);
    cv::moveWindow("Right Eye", 480, 600);
    cv::namedWindow("Left Eye",CV_WINDOW_NORMAL);
    cv::moveWindow("Left Eye", 480, 800);

    CvCapture *capture = cvCaptureFromCAM(-1);
    cv::Mat frame;
    if (capture) {
        while (true) {
            frame = cvQueryFrame(capture);
            if (frame.empty()) { std::cerr << "no captured frame." << std::endl; break; }
            cv::flip(frame, frame, 1);  // mirror
            detectAndDisplay(frame);
            cv::imshow(kWindowTitle, frame);
            if (static_cast<char>(cv::waitKey(10)) == 'c') { break; }
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

    if (kUseCascade) {
        std::vector<cv::Rect> eyesRegion;
        eyesCascade.detectMultiScale(faceROI, eyesRegion, 1.2, 2, CV_HAAR_SCALE_IMAGE, cv::Size(kMinEyes, kMinEyes));
        for (unsigned int j = 0; j < eyesRegion.size(); j++) {
            cv::Rect eyeRegion = eyesRegion[j];
            cv::Mat eyeROI = faceROI(eyeRegion);
            cv::Point center = findEyeCenter(eyeROI, "Left Eye");
            cv::circle(eyeROI, center, 3, 1234);
        }
    } else {
        findEyes(faceROI, faceRegion);
    }
}

void EyeTracker::findEyes(cv::Mat& faceROI, const cv::Rect &faceRegion) {
    int eyeRegionLeft   = faceRegion.width  * kEyeSide;
    int eyeRegionTop    = faceRegion.height * kEyeTop;
    int eyeRegionWidth  = faceRegion.width  * kEyeWidth;
    int eyeRegionHeight = faceRegion.height * kEyeHeight;
    int eyeRegionRight  = faceRegion.width  - eyeRegionWidth - eyeRegionLeft;

    cv::Rect leftEyeRegion(eyeRegionLeft, eyeRegionTop, eyeRegionWidth, eyeRegionHeight);
    cv::Rect rightEyeRegion(eyeRegionRight, eyeRegionTop, eyeRegionWidth, eyeRegionHeight);

    // draw eye region
    cv::rectangle(faceROI, leftEyeRegion, 1234);
    cv::rectangle(faceROI, rightEyeRegion, 1234);

    cv::Mat leftEyeROI = faceROI(leftEyeRegion);
    cv::Mat rightEyeROI = faceROI(rightEyeRegion);

    // find eye centers
    cv::Point leftCenter = findEyeCenter(leftEyeROI, "Left Eye");
    cv::Point rightCenter = findEyeCenter(rightEyeROI, "Right Eye");

    // change eye centers to face coordinates
    leftCenter.x += leftEyeRegion.x;
    leftCenter.y += leftEyeRegion.y;
    rightCenter.x += rightEyeRegion.x;
    rightCenter.y += rightEyeRegion.y;

    // draw eye centers
    cv::circle(faceROI, leftCenter, 3, 1234);
    cv::circle(faceROI, rightCenter, 3, 1234);
}

cv::Point EyeTracker::findEyeCenter(cv::Mat &eyeROI, std::string debugWindow) {
    float scaleRatio = static_cast<float>(kFastEyeWidth) / eyeROI.cols;
    cv::resize(eyeROI, eyeROI, cv::Size(kFastEyeWidth, scaleRatio * eyeROI.rows));  // scale down

    cv::Mat gradientX = computeMatXGradient(eyeROI);
    cv::Mat gradientY = computeMatXGradient(eyeROI.t()).t();
    cv::Mat magnitudes = matrixMagnitude(gradientX, gradientY);
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

//    cv::imshow(debugWindow, gradientX);

    // create a blurred and inverted image for weighting
    cv::Mat weight;
    cv::GaussianBlur(eyeROI, weight, cv::Size(kWeightBlurSize, kWeightBlurSize), 0, 0);
    for (int y = 0; y < weight.rows; y++) {
        unsigned char *row = weight.ptr<unsigned char>(y);
        for (int x = 0; x < weight.cols; x++) {
            row[x] = 255 - row[x];
        }
    }

    cv::imshow(debugWindow, weight);
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
            testPossibleCenters(x, y, wr[x], gx, gy, outSum);
        }
    }

    // scale all the values down, basically averaging them
    double numGradients = weight.rows * weight.cols;
    cv::Mat out;
    outSum.convertTo(out, CV_32F, 1.0 / numGradients);
    cv::Point maxP;
    cv::minMaxLoc(out, nullptr, nullptr, nullptr, &maxP);

    return cv::Point(roundf(maxP.x/scaleRatio), roundf(maxP.y)/scaleRatio);  // scale up
}

void EyeTracker::testPossibleCenters(int x, int y, unsigned char weight, double gx, double gy, cv::Mat &out) {
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
            double dotProduct = std::max(0.0, dx*gx + dy*gy);
            // square and multiply by the weight
            if (kEnableWeight) {
                Or[cx] += dotProduct * dotProduct * (weight/kWeightDivisor);
            } else {
                Or[cx] += dotProduct * dotProduct;
            }
        }
    }
}

cv::Mat EyeTracker::matrixMagnitude(const cv::Mat &matX, const cv::Mat &matY) {
    cv::Mat mags(matX.rows, matX.cols, CV_64F);
    for (int y = 0; y < matX.rows; y++) {
        const double *Xr = matX.ptr<double>(y);
        const double *Yr = matY.ptr<double>(y);
        double *Mr = mags.ptr<double>(y);
        for (int x = 0; x < matX.cols; x++) {
            double gX = Xr[x], gY = Yr[x];
            double magnitude = sqrt((gX * gX) + (gY * gY));
            Mr[x] = magnitude;
        }
    }
    return mags;
}

double EyeTracker::computeDynamicThreshold(const cv::Mat &mat, double stdDevFactor) {
    cv::Scalar stdMagnGrad, meanMagnGrad;
    cv::meanStdDev(mat, meanMagnGrad, stdMagnGrad);
    double stdDev = stdMagnGrad[0] / sqrt(mat.rows*mat.cols);
    return stdDevFactor * stdDev + meanMagnGrad[0];
}

cv::Mat EyeTracker::computeMatXGradient(const cv::Mat &mat) {
    cv::Mat out(mat.rows, mat.cols, CV_64F);
    for (int y = 0; y < mat.rows; y++) {
        const uchar *mr = mat.ptr<uchar>(y);
        double *Or = out.ptr<double>(y);

        Or[0] = mr[1] - mr[0];  // first
        for (int x = 1; x < mat.cols - 1; x++) {
            Or[x] = (mr[x+1] - mr[x-1]) / 2.0;
        }
        Or[mat.cols-1] = mr[mat.cols-1] - mr[mat.cols-2];  // last
    }
    return out;
}
