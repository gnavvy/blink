#include "EyeTracker.h"

EyeTracker::EyeTracker() {}

void EyeTracker::DetectAndDisplay(cv::Mat frame) {
    std::vector<cv::Rect> faces;
    std::vector<cv::Mat>  rgbChannels(3);
    cv::split(frame, rgbChannels);
    cv::Mat frame_gray = rgbChannels[2];
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2,
                                  0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT,
                                  cv::Size(150, 150));

    for (unsigned int i = 0; i < faces.size(); i++) {
        cv::rectangle(debugImage_, faces[i], 1234);
    }

    if (faces.size() > 0) {
        findEyes(frame_gray, faces[0]);
    }

}

void EyeTracker::Start() {
    if (!face_cascade.load(face_cascade_name)) {
        std::cerr << "cannot load face cascade file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string main_window_name = "Capture - Face detection";
    std::string face_window_name = "Capture - Face";
    cv::namedWindow(main_window_name,CV_WINDOW_NORMAL);
    cv::moveWindow(main_window_name, 400, 100);
    cv::namedWindow(face_window_name,CV_WINDOW_NORMAL);
    cv::moveWindow(face_window_name, 10, 100);
    cv::namedWindow("Right Eye",CV_WINDOW_NORMAL);
    cv::moveWindow("Right Eye", 10, 600);
    cv::namedWindow("Left Eye",CV_WINDOW_NORMAL);
    cv::moveWindow("Left Eye", 10, 800);

    createCornerKernels();

    cv::ellipse(skinCrCbHist, cv::Point(113, 155.6), cv::Size(23.4, 15.2),
                43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1);

    CvCapture *capture = cvCaptureFromCAM(-1);
    cv::Mat frame;
    if (capture) {
        while (true) {
            frame = cvQueryFrame(capture);
            cv::flip(frame, frame, 1);  // mirror
            frame.copyTo(debugImage_);

            if (!frame.empty()) {
                DetectAndDisplay(frame);
            } else {
                std::cerr << "no captured frame." << std::endl; break;
            }

            cv::imshow(main_window_name, debugImage_);

            int c = cv::waitKey(10);
            if (static_cast<char>(c) == 'c') break;
            if (static_cast<char>(c) == 'f') {
                cv::imwrite("frame.png",frame);
            }
        }
    }

    releaseCornerKernels();
}

void EyeTracker::findEyes(cv::Mat frame_gray, cv::Rect face) {
    cv::Mat faceROI = frame_gray(face);
    cv::Mat debugFace = faceROI;

    if (kSmoothFaceImage) {
        double sigma = kSmoothFaceFactor * face.width;
        cv::GaussianBlur( faceROI, faceROI, cv::Size( 0, 0 ), sigma);
    }

    // find eye regions and draw them
    int eyeRegionWidth = face.width * kEyePercentWidth / 100.0f;
    int eyeRegionHeight = face.width * kEyePercentHeight / 100.0f;  // ?
    int eyeRegionTop = face.height * kEyePercentTop / 100.0f;
    int eyeRegionLeft = face.width * kEyePercentSide / 100.0f;
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
        rightRightCornerRegion.y += rightRightCornerRegion.height / 2;\
    }

    cv::rectangle(debugFace,leftRightCornerRegion,200);
    cv::rectangle(debugFace,leftLeftCornerRegion,200);
    cv::rectangle(debugFace,rightLeftCornerRegion,200);
    cv::rectangle(debugFace,rightRightCornerRegion,200);

    // change eye centers to face coordinates
    leftPupil.x += leftEyeRegion.x;
    leftPupil.y += leftEyeRegion.y;
    rightPupil.x += rightEyeRegion.x;
    rightPupil.y += rightEyeRegion.y;

    std::cout << "left: " << leftPupil.x << ", " << leftPupil.y << std::endl;
    std::cout << "right:" << rightPupil.x << ", " << rightPupil.y << std::endl;

    // draw eye centers
    cv::circle(debugFace, rightPupil, 3, 1234);
    cv::circle(debugFace, leftPupil, 3, 1234);

    //-- Find Eye Corners
    if (kEnableEyeCorner) {
        cv::Point2f leftLeftCorner = findEyeCorner(faceROI(leftLeftCornerRegion), true, true); {
            leftLeftCorner.x += leftLeftCornerRegion.x;
            leftLeftCorner.y += leftLeftCornerRegion.y;
        }
        cv::Point2f leftRightCorner = findEyeCorner(faceROI(leftRightCornerRegion), true, false); {
            leftRightCorner.x += leftRightCornerRegion.x;
            leftRightCorner.y += leftRightCornerRegion.y;
        }
        cv::Point2f rightLeftCorner = findEyeCorner(faceROI(rightLeftCornerRegion), false, true); {
            rightLeftCorner.x += rightLeftCornerRegion.x;
            rightLeftCorner.y += rightLeftCornerRegion.y;
        }
        cv::Point2f rightRightCorner = findEyeCorner(faceROI(rightRightCornerRegion), false, false); {
            rightRightCorner.x += rightRightCornerRegion.x;
            rightRightCorner.y += rightRightCornerRegion.y;
        }

        cv::circle(faceROI, leftRightCorner, 3, 200);
        cv::circle(faceROI, leftLeftCorner, 3, 200);
        cv::circle(faceROI, rightLeftCorner, 3, 200);
        cv::circle(faceROI, rightRightCorner, 3, 200);
    }

    std::string face_window_name = "Capture - Face";
    cv::imshow(face_window_name, faceROI);
}

cv::Point EyeTracker::findEyeCenter(cv::Mat face, cv::Rect eye, std::string debugWindow) {
    cv::Mat eyeRoIunscaled = face(eye);
    cv::Mat eyeROI;
    scaleToFastSize(eyeRoIunscaled, eyeROI);
    // draw eye region
    cv::rectangle(face,eye,1234);

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
    std::cout << "eye size: " << outSum.cols << ", " << outSum.rows;
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

void EyeTracker::createCornerKernels() {
    leftCornerKernel = new cv::Mat(4, 6, CV_32F);
    rightCornerKernel = new cv::Mat(4, 6, CV_32F, kEyeCornerKernel);
    // flip horizontally
    cv::flip(*rightCornerKernel, *leftCornerKernel, 1);
}

void EyeTracker::releaseCornerKernels() {
    delete leftCornerKernel;
    delete rightCornerKernel;
}

cv::Mat EyeTracker::eyeCornerMap(const cv::Mat &region, bool left, bool left2) {
    cv::Mat cornerMap;
    cv::Size regionSize = region.size();
    cv::Range colRange(regionSize.width  / 4, regionSize.width  * 3 / 4);
    cv::Range rowRange(regionSize.height / 4, regionSize.height * 3 / 4);

    cv::Mat miRegion(region, rowRange, colRange);
    cv::filter2D(miRegion, cornerMap, CV_32F,
                 (left && !left2) || (!left && !left2) ? *leftCornerKernel : *rightCornerKernel);

    return cornerMap;
}

cv::Point2f EyeTracker::findSubpixelEyeCorner(cv::Mat region, cv::Point maxP) {
    cv::Size regionSize = region.size();
    cv::Mat cornerMap(regionSize.height * 10, regionSize.width * 10, CV_32F);
    cv::resize(region, cornerMap, cornerMap.size(), 0, 0, cv::INTER_CUBIC);
    cv::Point maxP2;
    cv::minMaxLoc(cornerMap, nullptr, nullptr, nullptr, &maxP2);
    return cv::Point2f(regionSize.width / 2 + maxP2.x / 10, regionSize.height / 2 + maxP2.y / 10);
}

cv::Point2f EyeTracker::findEyeCorner(cv::Mat region, bool left, bool left2) {
    cv::Mat cornerMap = eyeCornerMap(region, left, left2);

    cv::Point maxP;
    cv::minMaxLoc(cornerMap, nullptr, nullptr, nullptr, &maxP);

    cv::Point2f maxP2 = findSubpixelEyeCorner(cornerMap, maxP);
    return maxP2;
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
