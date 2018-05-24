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


inline void boxWrite(Mat& mat, String str, Point pt, Scalar boxCol);
inline void circz(Mat& mat, Point p, Scalar color, int radius);
inline void circz(Mat& mat, vector<Point> pts, Scalar color, int radius);
inline void cirLine(Mat& mat, Vec4i vec, Scalar color, int thickness, int offset);
inline void cirLine(Mat& mat, vector<Vec4i> vec, Scalar color, int thickness, int offset);
inline Mat col2gr(Mat mat);
inline Mat concatCols(Mat row1, Mat row2, Mat row3);
inline void cvtColAddLabel(vector<Mat>& vec, string* labels);
inline string decStr(double num);
inline void drawCVfitline(Mat& outImg, LaneLine lane);
inline void drawWarnArrows(Mat& out, double angle);
inline double getAngle(Vec4i line);
inline Mat concatRow(Mat col1, Mat col2, Mat col3);
inline double getSlope(Vec4i line);
inline Mat gr2col(Mat mat);
inline Mat grayMat(Mat img);
inline void linez(Mat& mat, Vec4i vec, Scalar color, int thickness, int offset);
inline void linez(Mat& mat, vector<Vec4i> vec, Scalar color, int thickness, int offset);
inline Vec4i pt2vec(Point a, Point b);
inline void setLinePts(int rows, int cols, LaneLine& lane);
inline void setLinePts(int rows, int cols, LaneLine& lane, LaneLine& lane2);
inline Mat reSz(Mat imgIn, double factor);
/**************************************************************************
 ***************************************************************************/
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
inline Mat reSz(Mat imgIn, double factor) {
	Mat imgOut;
	resize(imgIn, imgOut, Size(int(factor*imgIn.cols), int(factor*imgIn.rows)));
	return imgOut;
}
inline Vec4i pt2vec(Point a, Point b) {
	return Vec4i(a.x, a.y, b.x, b.y);
}

inline void boxWrite(Mat& mat, String str, Point pt, Scalar boxCol = black) {
	int bs = 0;
	Size sz = getTextSize(str, 4, .5, 1, &bs);
	rectangle(mat, Point(pt.x, pt.y + 2), Point(pt.x + sz.width, pt.y - sz.height - 1), boxCol, -1, 8);
	putText(mat, str, pt, /*FONT*/ 4, .5, white, 1, 8);
}

inline Mat gr2col(Mat mat) {
	Mat col;
	cvtColor(mat, col, COLOR_GRAY2BGR);
	return col;
}
inline Mat col2gr(Mat mat) {
	Mat gr;
	cvtColor(mat, gr, COLOR_BGR2GRAY);
	return gr;
}
inline Mat grayMat(Mat img) {
	return Mat(img.size(), img.type(), Scalar(64, 64, 64));
}

inline void linez(Mat& mat, Vec4i vec, Scalar color, int thickness = 1, int offset = 0) {
	line(mat, Point(vec[0], vec[1] + offset), Point(vec[2], vec[3] + offset), color, thickness);
}
inline void linez(Mat& mat, vector<Vec4i> vec, Scalar color, int thickness = 1, int offset = 0) {
	for (const Vec4i& z : vec)
		line(mat, Point(z[0], z[1] + offset), Point(z[2], z[3] + offset), color, thickness);
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
	circz(mat, Point(vec[0], vec[1]), color, thickness + 2);
	circz(mat, Point(vec[2], vec[3]), color, thickness + 2);
}
inline void cirLine(Mat& mat, vector<Vec4i> vec, Scalar color, int thickness = 1, int offset = 0) {
	for (const Vec4i& z : vec)
		cirLine(mat, z, color, thickness, offset);
}

inline Mat concatRow(Mat col1, Mat col2, Mat col3) {
	hconcat(col1, col2, col1);
	hconcat(col1, col3, col1);
	return col1;
}
inline Mat concatCols(Mat row1, Mat row2, Mat row3) {
	vconcat(row1, row2, row1);
	vconcat(row1, row3, row1);
	return row1;
}


inline void setLinePts(int rows, int cols, LaneLine& lane, LaneLine& lane2) {
	setLinePts(rows, cols, lane);
	setLinePts(rows, cols, lane2);
}
inline void setLinePts(int rows, int cols, LaneLine& lane) {
	if (lane.lines.empty())
		lane.aveFrLine = NULL;
	else {
		lane.aveFrLine = std::accumulate(lane.lines.begin(), lane.lines.end(), Vec4i(0, 0, 0, 0)) / int(lane.lines.size());		
		int xTop = int((lane.mod->y_offset - lane.aveFrLine[1]) / getSlope(lane.aveFrLine) + lane.aveFrLine[0]);
		lane.topPt = Point(xTop, lane.mod->y_offset);
		if (lane.color) {
			int yL = int(getSlope(lane.aveFrLine)*(-lane.aveFrLine[0])+lane.aveFrLine[1]);
			lane.botPt = (yL < rows) ? Point(0, yL) : Point(int((rows - yL) / getSlope(lane.aveFrLine)), rows);
		}
		else {
			int yR = int(getSlope(lane.aveFrLine)*(cols - lane.aveFrLine[0]) + lane.aveFrLine[1]);
			lane.botPt = (yR < rows) ? Point(cols, yR) : Point(int(((rows - yR) / getSlope(lane.aveFrLine)) + cols), rows);
		}
	}
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

inline void cvtColAddLabel(vector<Mat>& vec, string* labels) {
	for(int i = 0; i < vec.size(); i++) {
		if(vec[i].type() == 0) 
			cvtColor(vec[i], vec[i], COLOR_GRAY2BGR);
		//add gray space for label and then add label
		vconcat(vec[i], Mat(Size(vec[i].cols, 28), vec[i].type(), gray), vec[i]);
		boxWrite(vec[i], labels[i], Point(5, vec[i].rows - 8), gray);
	}
}
inline void drawCVfitline(Mat& outImg, LaneLine lane) {
	Vec4f fitline;
	vector<Point> ptVec;
	if (!lane.lines.empty()) {
		for (Vec4i z : lane.lines)
			ptVec.emplace_back(Point(z[0], z[1])), ptVec.emplace_back(Point(z[2], z[3]));
		fitLine(ptVec, fitline, 2, 0, 0.1, 0.1);
		double m = fitline[1] / fitline[0];
		Point2d b = Point2d(fitline[2], fitline[3]);
		double xIni = ((outImg.rows - b.y) / m) + b.x;
		double xFin = ((0 - b.y) / m) + b.x;
		Point a = Point(int(xIni), outImg.rows), c = Point(int(xFin), 0);
		linez(outImg, pt2vec(a, c), lane.color == RED ? white : yellow);
	}
}
#endif