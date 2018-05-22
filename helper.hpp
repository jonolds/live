#ifndef HELPER_HPP
#define HELPER_HPP
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include "Alg.h"
#include "colors.h"
using namespace std; using namespace cv;

inline string decStr(double num) {
	ostringstream obj;
	obj << ((abs(num) < 1) ? setprecision(2) : ((abs(num) < 10)? setprecision(3) : setprecision(4))) << num;
	return obj.str();
}

inline double getSlope(Vec4i line) {
	return (line[2] - line[0] != 0) ? double(line[3] - line[1]) / double(line[2] - line[0]) : 999.0;
}

inline double getAngle(Vec4i line) {
	return (getSlope(line) != 999) ? (atan(getSlope(line)) * 180) / CV_PI : 999.0;
}

inline void setLinePts(int rows, int cols, LaneLine*& l) {
	if (l->lines.empty())
		l->aveFrLine = Vec4i(0, 0, 0, 0);
	else {
		l->aveFrLine = std::accumulate(l->lines.begin(), l->lines.end(), Vec4i(0, 0, 0, 0)) / int(l->lines.size());		
		int xTop = int((l->mod->y_offset - l->aveFrLine[1]) / getSlope(l->aveFrLine) + l->aveFrLine[0]);
		l->topPt = Point(xTop, l->mod->y_offset);
		if (l->color) {
			int yL = int(getSlope(l->aveFrLine)*(-l->aveFrLine[0])+l->aveFrLine[1]);
			l->botPt = (yL < rows) ? Point(0, yL) : Point(int((rows - yL) / getSlope(l->aveFrLine)), rows);
		}
		else {
			int yR = int(getSlope(l->aveFrLine)*(cols - l->aveFrLine[0]) + l->aveFrLine[1]);
			l->botPt = (yR < rows) ? Point(cols, yR) : Point(int(((rows - yR) / getSlope(l->aveFrLine)) + cols), rows);
		}
		l->topPtDeq.emplace_front(l->topPt);
		l->botPtDeq.emplace_front(l->botPt);
		if (l->topPtDeq.size() > 3) {
			l->topPtDeq.pop_back();
			l->botPtDeq.pop_back();
		}
	}
}

inline void setLinePts(int rows, int cols, LaneLine*& lane, LaneLine*& lane2) {
	setLinePts(rows, cols, lane);
	setLinePts(rows, cols, lane2);
}

inline void Alg::drawLaneLines(LaneLine *lane, LaneLine *lane2) {
	drawLaneLines(lane);
	drawLaneLines(lane2);
}

inline void drawWarnArrows(Mat& out, double angle) {
	if (angle < 0) {
		arrowedLine(out, Point(60, 15), Point(20, 15), black, 7, FILLED, 0, 0.25);
		arrowedLine(out, Point(60, 15), Point(20, 15), green, 2, FILLED, 0, 0.25);
	} else {
		arrowedLine(out, Point(out.cols - 60, 15), Point(out.cols - 20, 15), black, 7, FILLED, 0, 0.25);
		arrowedLine(out, Point(out.cols - 60, 15), Point(out.cols - 20, 15), red, 2, FILLED, 0, 0.25);
	}
}

inline void boxWrite(Mat& mat, String str, Point pt, Scalar boxCol = black) {
	int bs = 0;
	Size sz = getTextSize(str, 4, .5, 1, &bs);
	rectangle(mat, Point(pt.x, pt.y + 2), Point(pt.x + sz.width, pt.y - sz.height - 1), boxCol, -1, 8);
	putText(mat, str, pt, /*FONT*/ 4, .5, white, 1, 8);
}

inline int getTurnsForSpeed(double speed) {
	//turns per speed (8x10^-5)x^4 - .0207x^3 + 1.9114x^2 - 78.976x + 1378.4
	return int((8*pow(10, -5)*pow(speed, 4)) - (.0207*pow(speed, 3)) + (1.9114*pow(speed, 2)) - (78.976*speed) + 1378.4);
}

inline Mat reSz(Mat imgIn, double factor) {
	Mat imgOut;
	resize(imgIn, imgOut, Size(int(factor*imgIn.cols), int(factor*imgIn.rows)));
	return imgOut;
}
inline void linez(Mat& mat, Vec4i vec, Scalar color, int thickness = 1, int offset = 0) {
	line(mat, Point(vec[0], vec[1]+offset), Point(vec[2], vec[3]+offset), color, thickness);
}

inline void linez(Mat& mat, vector<Vec4i> vec, Scalar color, int thickness = 1, int offset = 0) {
	for (const Vec4i& z : vec)
		line(mat, Point(z[0], z[1]+offset), Point(z[2], z[3]+offset), color, thickness);
}

inline void circz(Mat& mat, Point p, Scalar color, int radius = 3) {
	circle(mat, p, radius, color, -1, 8);
}

inline void circz(Mat& mat, vector<Point> pts, Scalar color, int radius = 3) {
	for (const Point& z : pts)
		circle(mat, z, radius, color, -1, 8);
}

inline void cirLine(Mat& mat, Vec4i vec, Scalar color, int thickness = 1, int offset = 0) {
	line(mat, Point(vec[0], vec[1] + offset), Point(vec[2], vec[3] + offset), color, thickness);
	circz(mat, Point(vec[0], vec[1]), color, thickness+2);
	circz(mat, Point(vec[2], vec[3]), color, thickness+2);
}
inline void cirLine(Mat& mat, vector<Vec4i> vec, Scalar color, int thickness = 1, int offset = 0) {
	for (const Vec4i& z : vec)
		cirLine(mat, z, color, thickness, offset);
}

inline Vec4i pt2vec(Point a, Point b) {
	return Vec4i(a.x, a.y, b.x, b.y);
}

inline void addLabel(Mat& img, string text) {
	vconcat(img, Mat(Size(img.cols, 28), img.type(), gray), img);
	boxWrite(img, text, Point(5, img.rows - 8), gray);
}

inline void cvtColAddLabel(vector<Mat>& vec, string* labels) {
	for(int i = 0; i < vec.size(); i++) {
		if(vec.at(i).type() == 0) 
			cvtColor(vec.at(i), vec.at(i), COLOR_GRAY2BGR);
		addLabel(vec.at(i), labels[i]);
	}
}

inline void Alg::showImages() {
	Mat row1, row2, row3;
	vector<Mat> mats= {inSmall, grayImg, blurImg, maskImg, edgeImg, houghImg, outFrame};
	string labels[] = { "inSmall", "gray", "blur", "mask", "edge", "hough", "outFrame" };
	cvtColAddLabel(mats, labels);
	hconcat(mats.at(0), mats.at(1), row1);
	hconcat(mats.at(2), mats.at(3), row2);
	hconcat(mats.at(4), mats.at(5), row3);
	vconcat(row1, row2, row1);
	vconcat(row1, row3, row1);
	imshow("inSmall - grayImg - blurImg - edge - hough - outFrame", reSz(row1, .8));
	waitKey(25);
}

inline Mat superBlur(Mat grayImg) {
	Mat blurImg = grayImg.clone();
	blur(blurImg, blurImg, Size(5, 5));
	GaussianBlur(blurImg, blurImg, Size(7, 7), 4);
	GaussianBlur(blurImg, blurImg, Size(5, 7), 4);
	return blurImg;
}

inline void Alg::cleanup() {
	delete gTop;
	delete rTop;
}

inline void drawCVfitline(Mat& outImg, LaneLine* lane) {
	Vec4f fitline;
	vector<Point> ptVec;
	if (!lane->lines.empty()) {
		for (Vec4i z : lane->lines)
			ptVec.emplace_back(Point(z[0], z[1])), ptVec.emplace_back(Point(z[2], z[3]));
		fitLine(ptVec, fitline, 2, 0, 0.1, 0.1);
		double m = fitline[1] / fitline[0];
		Point2d b = Point2d(fitline[2], fitline[3]);
		double xIni = ((outImg.rows - b.y) / m) + b.x;
		double xFin = ((0 - b.y) / m) + b.x;
		Point a = Point(int(xIni), outImg.rows), c = Point(int(xFin), 0);
		linez(outImg, pt2vec(a, c), lane->color == RED ? white : yellow);
	}
}
#endif