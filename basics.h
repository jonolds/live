#ifndef BASICS_H
#define BASICS_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "constants.h"
#include "t7.h"


void boxWrite(cv::Mat& mat, std::string str, Pt pt, cv::Scalar boxCol = black);
cv::Mat cat2by2(cv::Mat img1, cv::Mat img2, cv::Mat img3, cv::Mat img4);
cv::Mat cat2by3(cv::Mat img1, cv::Mat img2, cv::Mat img3, cv::Mat img4, cv::Mat img5, cv::Mat img6);
cv::Mat catCols(cv::Mat row1, cv::Mat row2, cv::Mat row3);
cv::Mat catRows(cv::Mat col1, cv::Mat col2, cv::Mat col3);
cv::Mat cvtCol(cv::Mat img);
cv::Mat cvtGr(cv::Mat img);
void show(cv::Mat img, int time = 0, std::string winName = "default name");
std::string decStr(double num);
void drawArrow(cv::Mat& out, bool neg);
cv::Mat getSolidImg(cv::Mat img, cv::Scalar color);
cv::Mat rectangleRet(cv::Mat img, Pt p1, Pt p2, cv::Scalar color, int thickness = -1);
cv::Mat reSz(cv::Mat img, double factor);
#endif