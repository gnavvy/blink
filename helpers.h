#ifndef HELPERS_H
#define HELPERS_H

#include <queue>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "constants.h"

bool rectInImage(cv::Rect rect, cv::Mat image);
bool inMat(cv::Point p,int rows,int cols);
cv::Mat matrixMagnitude(const cv::Mat &matX, const cv::Mat &matY);
double computeDynamicThreshold(const cv::Mat &mat, double stdDevFactor);
cv::Point unscalePoint(cv::Point p, cv::Rect origSize);
void scaleToFastSize(const cv::Mat &src, cv::Mat &tgt);
cv::Mat computeMatXGradient(const cv::Mat &mat);
bool floodShouldPushPoint(const cv::Point &np, const cv::Mat &mat);
cv::Mat floodKillEdges(cv::Mat &mat);

#endif // HELPERS_H
