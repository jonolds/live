#ifndef _HELPER_HPP_
#define  _HELPER_HPP_
#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
#include <math.h>
#include <iomanip>
using namespace std; using namespace cv; using namespace viz;

Size getSize(VideoCapture vc) {
	return Size(int(vc.get(CAP_PROP_FRAME_WIDTH)), int(vc.get(CAP_PROP_FRAME_HEIGHT)));
}
void setSize(VideoCapture reader, Size reqSize) {
	reader.set(CAP_PROP_FRAME_WIDTH, reqSize.width);
	reader.set(CAP_PROP_FRAME_HEIGHT, reqSize.height);
}

void drawWarnArrows(Mat & out, double angle) {
	if (angle < 0) {
		arrowedLine(out, Point(60, 15), Point(20, 15), Color::black(), 7, FILLED, 0, 0.25);
		arrowedLine(out, Point(60, 15), Point(20, 15), Color::green(), 2, FILLED, 0, 0.25);
	}
	if (angle > 0) {
		arrowedLine(out, Point(out.cols - 60, 15), Point(out.cols - 20, 15), Color::black(), 7, FILLED, 0, 0.25);
		arrowedLine(out, Point(out.cols - 60, 15), Point(out.cols - 20, 15), Color::red(), 2, FILLED, 0, 0.25);
	}
}

void boxWrite(Mat& mat, String str, Point pt) {
	int bs = 0;
	Size sz = getTextSize(str, 4, .5, 1, &bs);
	rectangle(mat, Point(pt.x, pt.y + 2), Point(pt.x + sz.width, pt.y - sz.height - 1), Color::black(), -1, 8);
	putText(mat, str, pt, /*FONT*/ 4, .5, Color::white(), 1, 8);
}
double getSlope(Vec4i line) {
	int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
	double slope = x2 - x1 != 0 ? double(y2 - y1) / double(x2 - x1) : 999;
	return slope;
}
double getAngle(Vec4i line, int& x1, int& y1, int& x2, int&y2) {
	x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
	double slope = x2 - x1 != 0 ? double(y2 - y1) / double(x2 - x1) : 999;
	double angle = slope != 999 ? (atan(slope) * 180) / CV_PI : 999.0;
	return angle;
}
double getAngle(Vec4i line) {
	int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
	double slope = x2 - x1 != 0 ? double(y2 - y1) / double(x2 - x1) : 999;
	double angle = slope != 999 ? (atan(slope) * 180) / CV_PI : 999.0;
	return angle;
}

string decStr(double num) {
	ostringstream obj;
	if (abs(num) < 1)
		obj << std::setprecision(2) << num;
	else if (abs(num) < 10)
		obj << std::setprecision(3) << num;
	else
		obj << std::setprecision(4) << num;
	return obj.str();
}
void printLineInf(vector<Vec4i> greens, vector<Vec4i> reds, vector<Vec4i> greenFrameLines, vector<Vec4i> redFrameLines, Scalar greensMean, Scalar redsMean, Scalar greensStdDev, Scalar redsStdDev) {
	cout << "greens:\n" << Mat(greens) << "\n\nGreens pruned:\n" << Mat(greenFrameLines) << "\n";
	cout << "greensMean:\n" << Mat(greensMean) << "\n\ngreensStdDev: \n" << Mat(greensStdDev) << "\n";
	cout << "reds:\n" << Mat(reds) << "\n\nReds pruned:\n" << Mat(redFrameLines) << "\n";
	cout << "redsMean:\n" << Mat(redsMean) << "\n\nredsStdDev: \n" << Mat(redsStdDev) << "\n";
	cout << "greenCount: " << Mat(greenFrameLines).rows << "   redCount: " << Mat(redFrameLines).rows << "\n";
	//cout << "Total greenCount: " << greenCount << "   Total redCount: " << redCount << "\n\n";
}
#endif