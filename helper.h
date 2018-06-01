#ifndef HELPER_H
#define HELPER_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "t7vec.h"

cv::Mat getVanImg(cv::Mat img, t7 gAve, t7 rAve, cv::Point vanish);
cv::Mat superBlur(cv::Mat img);
cv::Mat canny(cv::Mat blrImg, double lowThrsh, double highThrsh);
cv::Mat drawHoughLines(cv::Mat img, t7vec gLns, t7vec rLns, t7vec badLns);
#endif
