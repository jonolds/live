#ifndef HELPER_H
#define HELPER_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "Sector.h"

cv::Mat canny(cv::Mat blrImg, double lowThrsh, double highThrsh);
cv::Mat drawHoughLines(cv::Mat img, Sector gLns, Sector rLns, Sector badLns);
cv::Mat getVanImg(cv::Mat img, t7 gAve, t7 rAve, cv::Point vanish);
Sector HoughLinesP_t7vec(cv::Mat mat, double rho, double theta, int threshold, double minLen = 0, double maxGap = 0);
cv::Mat superBlur(cv::Mat img);

#endif
