#ifndef HELPER_HPP
#define HELPER_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <cmath>
#include <numeric>
#include "t7vec.hpp"
#include "basics.hpp"

using namespace std;
using namespace cv;

void boxWrite(Mat& mat, String str, Point pt, Scalar boxCol);
void dot(Mat& mat, Point p, Scalar color, int radius);
void dotLns(Mat& mat, t7vec tVec, Scalar color, int y_off = 0, int thickness = 1);
Mat getVanImg(Mat img, t7 gAve, t7 rAve, Point vanish);
void label(vector<Mat>& vec, string* labels);
void ln(Mat& mat, t7 t, Scalar color, int thickness = 1, int offset = 0);
Mat superBlur(Mat img);
Mat canny(Mat blrImg, double lowThrsh, double highThrsh);
Mat mask(Mat mask, Scalar color, int y_offset, int rows, int cols);
Mat drawHoughLines(Mat img, t7vec gLns, t7vec rLns, t7vec badLns);
/**************************************************************************/

inline void boxWrite(Mat& mat, String str, Point pt, Scalar boxCol = black) {
	int bs = 0;
	Size sz = getTextSize(str, 4, .5, 1, &bs);
	rectangle(mat, Point(pt.x, pt.y + 2), Point(pt.x + sz.width, pt.y - sz.height - 1), boxCol, -1, 8);
	putText(mat, str, pt, /*FONT*/ 4, .5, white, 1, 8);
}

inline void dot(Mat& mat, Point p, Scalar color, int radius = 3) {
	circle(mat, p, radius, color, -1, 8);
}
inline void dotLns(Mat& mat, t7vec tVec, Scalar color, int y_off, int thickness) {
	for(t7 t: tVec) {
		line(mat, Point(t[0], t[1] + y_off), Point(t[2], t[3] + y_off), color, thickness);
		dot(mat, t.p1(), color, thickness + 2);
		dot(mat, t.p2(), color, thickness + 2);
	}
}

inline Mat getVanImg(Mat img, t7 gAve, t7 rAve, Point vanish) {
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
inline void label(vector<Mat>& vec, string* labels) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i].type() == 0)
			cvtColor(vec[i], vec[i], COLOR_GRAY2BGR);
		//add gray space for label and then add label
		vconcat(vec[i], Mat(Size(vec[i].cols, 28), vec[i].type(), gray), vec[i]);
		boxWrite(vec[i], labels[i], Point(5, vec[i].rows - 8), gray);
	}
}
inline void ln(Mat& mat, t7 t, Scalar color, int thickness, int offset) {
		line(mat, Point(t[0], t[1] + offset), Point(t[2], t[3] + offset), color, thickness);
}

inline Mat mask(Mat mask, Scalar color, int y_offset, int rows, int cols) {
	rectangle(mask, Point(0, y_offset), Point(cols, 0), color, -1);
	Point rhombus[4] = { Point(int(.1*cols), rows), Point(int(.4*cols), y_offset), Point(int(.6*cols), y_offset), Point(int(.9*cols), rows) };
	fillConvexPoly(mask, rhombus, 4, color);
	return mask;
}

inline Mat drawHoughLines(Mat img, t7vec gLns, t7vec rLns, t7vec badLns) {
	gLns.drwVecLns(img);
	rLns.drwVecLns(img);
	badLns.drwVecLns(img);
	return img;
}

inline Mat superBlur(Mat img) {
	blur(img, img, Size(5, 5));
	GaussianBlur(img, img, Size(7, 7), 4);
	GaussianBlur(img, img, Size(5, 7), 4);
	return img;
}
inline Mat canny(Mat blrImg, double lowThrsh, double highThrsh) {
	Canny(blrImg, blrImg, lowThrsh, highThrsh);
	return blrImg;
}
#endif