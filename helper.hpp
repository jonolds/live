#ifndef HELPER_HPP
#define HELPER_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include "Alg.h"
#include "colors.h"

using namespace std;
using namespace cv;

void boxWrite(Mat& mat, String str, Point pt, Scalar boxCol);
Mat catCols(Mat row1, Mat row2, Mat row3);
Mat catRows(Mat col1, Mat col2, Mat col3);
void dotLn(Mat& mat, t7i t, Scalar color, int y_off = 0, int thickness = 1);
void dotLns(Mat& mat, vecT7 tVec, Scalar color, int y_off = 0, int thickness = 1);
void dot(Mat& mat, Point p, Scalar color, int radius);
void dots(Mat& mat, vector<Point> pts, Scalar color, int radius);
Mat cvtCol2gr(Mat mat);
void label(vector<Mat>& vec, string* labels);
Mat cvtGr2col(Mat mat);
string decStr(double num);
void drawArrow(Mat& out, bool neg);
void drawLineByIter(Mat& img, Point a, Point b, Scalar color);
t7i getAvLn(vecT7 tVec);
t7i getCVfitline(vecT7 lines);
Mat getSolidImg(Mat img, Scalar color);
Mat getVanImg(Mat img, t7i gAve, t7i rAve);
void ln(Mat& mat, t7i t, Scalar color, int thickness = 1, int offset = 0);
void lns(Mat& mat, vecT7 tVec, Scalar color, int thickness = 1, int offset = 0);
Mat reSz(Mat imgIn, double factor);

Mat superBlur(Mat img);
Mat canny(Mat blrImg, double lowThrsh, double highThrsh);
Mat mask(Mat cnnyImg, Scalar color, int y_offset, int rows, int cols);
void sortHoughLines(Alg& alg);
Mat drawHoughLines(Mat img, vecT7 gLns, vecT7 rLns, vecT7 badLns, int yOff);
/**************************************************************************/

inline void boxWrite(Mat& mat, String str, Point pt, Scalar boxCol = black) {
	int bs = 0;
	Size sz = getTextSize(str, 4, .5, 1, &bs);
	rectangle(mat, Point(pt.x, pt.y + 2), Point(pt.x + sz.width, pt.y - sz.height - 1), boxCol, -1, 8);
	putText(mat, str, pt, /*FONT*/ 4, .5, white, 1, 8);
}
inline Mat catCols(Mat row1, Mat row2, Mat row3) {
	vconcat(row1, row2, row1);
	vconcat(row1, row3, row1);
	return row1;
}
inline Mat catRows(Mat col1, Mat col2, Mat col3) {
	hconcat(col1, col2, col1);
	hconcat(col1, col3, col1);
	return col1;
}
inline void dotLn(Mat& mat, t7i t, Scalar color, int y_off, int thickness) {
	line(mat, Point(t[0], t[1] + y_off), Point(t[2], t[3] + y_off), color, thickness);
	dot(mat, t.p1(), color, thickness + 2);
	dot(mat, t.p2(), color, thickness + 2);
}
inline void dotLns(Mat& mat, vecT7 tVec, Scalar color, int y_off, int thickness) {
	for (const t7i& z : tVec)
		dotLn(mat, z, color, thickness, y_off);
}
inline void dot(Mat& mat, Point p, Scalar color, int radius = 3) {
	circle(mat, p, radius, color, -1, 8);
}
inline void dots(Mat& mat, vector<Point> pts, Scalar color, int radius = 3) {
	for (const Point& z : pts)
		circle(mat, z, radius, color, -1, 8);
}
inline Mat cvtCol2gr(Mat mat) {
	cvtColor(mat, mat, COLOR_BGR2GRAY);
	return mat;
}
inline void label(vector<Mat>& vec, string* labels) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i].type() == 0)
			cvtColor(vec[i], vec[i], COLOR_GRAY2BGR);
		//add gray space for label and then add label
		vconcat(vec[i], Mat(Size(vec[i].cols, 28), vec[i].type(), gray), vec[i]);
		boxWrite(vec[i], labels[i], Point(5, vec[i].rows - 8), gray);
	}
}
inline Mat cvtGr2col(Mat mat) {
	cvtColor(mat, mat, COLOR_GRAY2BGR);
	return mat;
}
inline string decStr(double num) {
	ostringstream obj;
	obj << ((abs(num) < 1) ? setprecision(2) : ((abs(num) < 10)? setprecision(3) : setprecision(4))) << num;
	return obj.str();
}
inline void drawArrow(Mat& out, bool neg) {
	arrowedLine(out, Point((neg) ? 60 : out.cols - 60, 15), Point((neg) ? 20 : out.cols - 20, 15), black, 7, FILLED, 0, 0.25);
	arrowedLine(out, Point((neg) ? 60 : out.cols - 60, 15), Point((neg) ? 20 : out.cols - 20, 15), green, 2, FILLED, 0, 0.25);
}
inline void drawLineByIter(Mat& img, Point a, Point b, Scalar color) {
	LineIterator lit(img, a, b);
	for(int i = 0; i < lit.count; i++, ++lit) {
		Vec3b& px = img.at<Vec3b>(lit.pos());
		px = Vec3b(uchar(color[0]), uchar(color[1]), uchar(color[2]));
	}	
}

inline t7i getAvLn(vecT7 tVec) {
	return std::accumulate(tVec.begin(), tVec.end(), t7i(0, 0, 0, 0)) / int(tVec.size());
}
inline t7i getCVfitline(vecT7 lines) {
	t7i fitline;
	vector<Point> ptVec;
	for (t7i z : lines)
		ptVec.emplace_back(Point(z[0], z[1])), ptVec.emplace_back(Point(z[2], z[3]));
	fitLine(ptVec, fitline, 2, 0, 0.1, 0.1);
	return fitline;
}

inline Mat getSolidImg(Mat img, Scalar color) {
	return Mat(img.size(), img.type(), color);
}
inline Mat getVanImg(Mat img, t7i gAve, t7i rAve, Point vanish) {
	Mat tmpGr = getSolidImg(img, black);
	vconcat(tmpGr, img, tmpGr);
	line(tmpGr, Point(gAve[2], gAve[3] + img.rows), vanish + Point(0, img.rows), white, 2);
	line(tmpGr, Point(rAve[0], rAve[1] + img.rows), Point(vanish.x, vanish.y + img.rows), white, 2);
	circle(tmpGr, Point(vanish.x, vanish.y + img.rows), 3, blue, -1);
	string s[] = { "vanish", " " };
	vector<Mat> m = { tmpGr, getSolidImg(img, gray) };
	label(m, s);
	vconcat(m[0], m[1], m[0]);
	vconcat(Mat(Size(m[0].cols, 28), m[0].type(), gray), m[0], m[0]);
	return m[0];
}

inline void ln(Mat& mat, t7i t, Scalar color, int thickness, int offset) {
	line(mat, Point(t[0], t[1] + offset), Point(t[2], t[3] + offset), color, thickness);
}
inline void lns(Mat& mat, vecT7 tVec, Scalar color, int thickness, int offset) {
	for (t7i& z : tVec)
		line(mat, Point(z[0], z[1] + offset), Point(z[2], z[3] + offset), color, thickness);
}
inline Mat reSz(Mat imgIn, double factor) {
	Mat imgOut;
	resize(imgIn, imgOut, Size(int(factor*imgIn.cols), int(factor*imgIn.rows)));
	return imgOut;
}

inline Mat superBlur(Mat img) {
	Mat blurred = img.clone();
	blur(blurred, blurred, Size(5, 5));
	GaussianBlur(blurred, blurred, Size(7, 7), 4);
	GaussianBlur(blurred, blurred, Size(5, 7), 4);
	return blurred;
}

inline Mat canny(Mat blrImg, double lowThrsh, double highThrsh) {
	Mat canImg;
	Canny(blrImg, canImg, lowThrsh, highThrsh);
	return canImg;
}
inline Mat mask(Mat cnnyImg, Scalar color, int y_offset, int rows, int cols) {
	Mat mask = cnnyImg.clone();
	rectangle(mask, Point(0, y_offset), Point(cols, 0), color, -1);
	Point rhombus[4] = { Point(int(.1*cols), rows), Point(int(.4*cols), y_offset), Point(int(.6*cols), y_offset), Point(int(.9*cols), rows) };
	fillConvexPoly(mask, rhombus, 4, color);
	return mask;
}
inline void sortHoughLines(Alg& alg) {
	for (t7i& t : alg.lns) {
		if ((t.ang > 15) && (t.ang < 65) && (t[0] > alg.topMidRPt.x) && (t[2] > alg.topMidRPt.x))
			alg.rLns.emplace_back(t);
		else if ((t.ang < 15) && (t.ang < 65) && (t[0] < alg.topMidLPt.x) && (t[2] < alg.topMidLPt.x))
			alg.gLns.emplace_back(t);
		else
			alg.badLns.emplace_back(t);
	}
}
inline Mat drawHoughLines(Mat img, vecT7 gLns, vecT7 rLns, vecT7 badLns, int yOff) {
	Mat hImg = img.clone();
	for (t7i& t : gLns)
		line(hImg, Point(t[0], t[1] + yOff), Point(t[2], t[3] + yOff), green);
	for (t7i& t : rLns)
		line(hImg, Point(t[0], t[1] + yOff), Point(t[2], t[3] + yOff), red);
	for (t7i& t : badLns)
		line(hImg, Point(t[0], t[1] + yOff), Point(t[2], t[3] + yOff), white);
	return hImg;
}
#endif