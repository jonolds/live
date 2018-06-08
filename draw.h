#ifndef DRAW_H
#define DRAW_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "helper.h"

void dot(cv::Mat& mat, cv::Point p, cv::Scalar color, int radius = 3);
void dotLns(cv::Mat& mat, t7vec tVec, cv::Scalar color, int y_off = 0, int thickness = 1);
void label(std::vector<cv::Mat>& vec, std::string* labels);
void ln(cv::Mat& mat, t7 t, cv::Scalar color, int thickness = 1, int offset = 0);
std::vector<cv::Mat> makeGrid(cv::Mat img, int yOff);
#endif