#ifndef BASICS_HPP
#define BASICS_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "colors.h"

void boxWrite(Mat& mat, std::string str, Point pt, Scalar boxCol = black);
Mat catCols(Mat row1, Mat row2, Mat row3);
Mat catRows(Mat col1, Mat col2, Mat col3);
Mat cvtColGr(Mat mat);
std::string decStr(double num);
void drawArrow(Mat& out, bool neg);
Mat getSolidImg(Mat img, Scalar color);
Mat reSz(Mat img, double factor);
#endif
