#ifndef HELPER_H
#define HELPER_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "t7vec.h"

cv::Mat canny(cv::Mat blrImg, double lowThrsh, double highThrsh);
cv::Mat drawHoughLines(cv::Mat img, t7vec gLns, t7vec rLns, t7vec badLns);
cv::Mat getVanImg(cv::Mat img, t7 gAve, t7 rAve, cv::Point vanish);
t7vec HoughLinesP_t7vec(cv::Mat mat, double rho, double theta, int threshold, double minLen = 0, double maxGap = 0);
cv::Mat superBlur(cv::Mat img);
#endif
