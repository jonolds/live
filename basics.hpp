#ifndef BASICS_HPP
#define BASICS_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <cmath>
#include <iomanip>
#include <numeric>
#include "colors.h"

using namespace std;
using namespace cv;

Mat catCols(Mat row1, Mat row2, Mat row3);
Mat catRows(Mat col1, Mat col2, Mat col3);
Mat cvtColGr(Mat mat);
string decStr(double num);
void drawArrow(Mat& out, bool neg);
Mat getSolidImg(Mat img, Scalar color);
Mat reSz(Mat img, double factor);

/**************************************************************************/

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
inline Mat cvtColGr(Mat mat) {
	cvtColor(mat, mat, (mat.type() == 16) ? COLOR_BGR2GRAY : COLOR_GRAY2BGR);
	return mat;
}
inline string decStr(double num) {
	ostringstream obj;
	obj << ((abs(num) < 1) ? setprecision(2) : ((abs(num) < 10) ? setprecision(3) : setprecision(4))) << num;
	return obj.str();
}

inline void drawArrow(Mat& out, bool neg) {
	arrowedLine(out, Point((neg) ? 60 : out.cols - 60, 15), Point((neg) ? 20 : out.cols - 20, 15), black, 7, FILLED, 0, 0.25);
	arrowedLine(out, Point((neg) ? 60 : out.cols - 60, 15), Point((neg) ? 20 : out.cols - 20, 15), green, 2, FILLED, 0, 0.25);
}
inline Mat getSolidImg(Mat img, Scalar color) {
	return Mat(img.size(), img.type(), color);
}
inline void drawLineByIter(Mat& img, Point a, Point b, Scalar color) {
	LineIterator lit(img, a, b);
	for (int i = 0; i < lit.count; i++, ++lit) {
		Vec3b& px = img.at<Vec3b>(lit.pos());
		px = Vec3b(uchar(color[0]), uchar(color[1]), uchar(color[2]));
	}
}
inline Mat reSz(Mat img, double factor) {
	resize(img, img, Size(int(factor*img.cols), int(factor*img.rows)));
	return img;
}
#endif