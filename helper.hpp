#ifndef HELPER_HPP
#define  HELPER_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
#include <math.h>
#include <iomanip>
#include <numeric>
#include "Alg.h"
using namespace std; using namespace cv; using namespace viz;

string decStr(double num) {
	ostringstream obj;
	obj << ((abs(num) < 1) ? setprecision(2) : ((abs(num) < 10)? setprecision(3) : setprecision(4))) << num;
	return obj.str();
}
double getSlope(Vec4i line) {
	return (line[2] - line[0] != 0) ? double(line[3] - line[1]) / double(line[2] - line[0]) : 999.0;
}
double getAngle(Vec4i line, int& x1, int& y1, int& x2, int&y2) {
	x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
	return (getSlope(line) != 999) ? (atan(getSlope(line)) * 180) / CV_PI : 999;
}
double getAngle(Vec4i line) {
	return (getSlope(line) != 999) ? (atan(getSlope(line)) * 180) / CV_PI : 999.0;
}
void setLinePts(Mat out, LaneLine& lane) {
	if (lane.lines.size() == 0)
		lane.aveLine = Vec4i(0, 0, 0, 0);
	else {
		lane.aveLine = std::accumulate(lane.lines.begin(), lane.lines.end(), Vec4i(0, 0, 0, 0)) / int(lane.lines.size());		
		int xTop = int((lane.mod->y_offset - lane.aveLine[1]) / getSlope(lane.aveLine) + lane.aveLine[0]);
		lane.topPt = Point(xTop, lane.mod->y_offset);
		if (lane.color) {
			int yL = int(getSlope(lane.aveLine)*(-lane.aveLine[0])+lane.aveLine[1]);
			lane.botPt = (yL < out.rows) ? Point(0, yL) : Point(int((out.rows - yL) / getSlope(lane.aveLine)), out.rows);
		}
		else {
			int yR = int(getSlope(lane.aveLine)*(out.cols - lane.aveLine[0]) + lane.aveLine[1]);
			lane.botPt = (yR < out.rows) ? Point(out.cols, yR) : Point(int(((out.rows - yR) / getSlope(lane.aveLine)) + out.cols), out.rows);
		}
		lane.topPtDeq.emplace_front(lane.topPt);
		lane.botPtDeq.emplace_front(lane.botPt);
		if (lane.topPtDeq.size() > 3) {
			lane.topPtDeq.pop_back();
			lane.botPtDeq.pop_back();
		}
	}
}

void drawWarnArrows(Mat & out, double angle) {
	if (angle < 0) {
		arrowedLine(out, Point(60, 15), Point(20, 15), Color::black(), 7, FILLED, 0, 0.25);
		arrowedLine(out, Point(60, 15), Point(20, 15), Color::green(), 2, FILLED, 0, 0.25);
	} else {
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
#endif