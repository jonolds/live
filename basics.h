#ifndef BASICS_HPP
#define BASICS_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "colors.h"
typedef Point Pt;
void boxWrite(Mat& mat, std::string str, Point pt, Scalar boxCol = black);
Mat cat2by2(Mat img1, Mat img2, Mat img3, Mat img4);
Mat cat2by3(Mat img1, Mat img2, Mat img3, Mat img4, Mat img5, Mat img6);
Mat catCols(Mat row1, Mat row2, Mat row3);
Mat catRows(Mat col1, Mat col2, Mat col3);
Mat cvtCol(Mat img);
Mat cvtGr(Mat img);
void show(Mat img, int time = 0, string winName = "default name");
std::string decStr(double num);
void drawArrow(Mat& out, bool neg);
Mat getSolidImg(Mat img, Scalar color);
Mat rectangleRet(Mat img, Point p1, Point p2, Scalar color, int thickness = -1);
Mat reSz(Mat img, double factor);
#endif