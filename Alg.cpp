#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
#include <math.h>
#include <iomanip>
#include <numeric>
#include "Alg.h"
#include "helper.hpp"
using namespace std; using namespace cv; using namespace viz;

void Alg::process(Mat& src, Mat& out) {
	/*Canny*/lowThr = 30, highThr = 105, l2 = false, /*Hough*/ hThresh = 15, minLen = 20, maxGap = 70;
	gAveLine = Vec4i::all(0); rAveLine = Vec4i::all(0);
	vector<Vec4i> lines, greens, reds, greenFrameLines, redFrameLines;

	Mat src_gray = init(src, out);
	y_offset = int(offsetFactor * out.rows);

	Mat edges = applyCanny(src_gray, out);
	applyHoughP(edges, out, lines);

	sortLines(out, lines, greens, reds);
	drawLines(out, greens, reds);
	drawMarks(out);
}
Mat Alg::applyCanny(Mat src_gray, Mat& out) {
	blur(src_gray, src_gray, Size(5, 5));
	GaussianBlur(src_gray, src_gray, Size(7, 7), 4);
	GaussianBlur(src_gray, src_gray, Size(5, 7), 4);
	Mat edges(src_gray.size(), CV_8U, Scalar::all(0));
	Canny(src_gray, edges, lowThr, highThr, 3, l2);

	rectangle(edges, Point(0, y_offset), Point(src_gray.cols, 0), Scalar::all(0), -1);
	Point triangle[3] = { Point(int(.1*edges.cols), edges.rows), Point(int(.9*edges.cols), edges.rows), Point(int(.5*edges.cols), y_offset) };
	fillConvexPoly(edges, triangle, 3, Scalar::all(255));

	imshow("canny", edges);
	fillConvexPoly(edges, triangle, 3, Scalar::all(0));
	fillConvexPoly(out, triangle, 3, Scalar::all(255));//color white triangle on out
	return edges;
}
void Alg::applyHoughP(Mat& edges, Mat& out, vector<Vec4i>& lines) {
	HoughLinesP(edges, lines, 1, 1 * CV_PI / 180, hThresh, minLen, maxGap); //1st best{30, 20, 20}>{30,10,20}>{40, 20, 10} 
	line(out, Point(0, y_offset), Point(out.size().width, y_offset), Color::pink(), 1, 8);
}
void Alg::sortLines(Mat& out, vector<Vec4i>& lines, vector<Vec4i>& greens, vector<Vec4i>& reds) {
	for (size_t i = 0; i < lines.size(); i++) {
		int x1, y1, x2, y2;
		double angle = getAngle(lines[i], x1, y1, x2, y2);
		if ((abs(angle) > 20) && (abs(angle) < 62)) {
			if ((angle > 0) && (x1 > topMidRightP.x) && (x2 > topMidRightP.x))
				reds.emplace_back(lines[i]);
			else if ((angle < 0) && (x1 < topMidLeftP.x) && (x2 < topMidLeftP.x))
				greens.emplace_back(lines[i]);
		}
	}
	if (greens.size() == 0) {
		if (gTopDeq.at(0) != Point(0,0)) {
			gTopDeq.emplace_front(gTopDeq.at(0));
			gBotDeq.emplace_front(gBotDeq.at(0));
		}
		gAveLine = Vec4i(0,0,0,0);
	}
	else {
		gAveLine = std::accumulate(greens.begin(), greens.end(), Vec4i(0, 0, 0, 0)) / int(greens.size());
		//green top/bottom pts and line
		int xTop = int((y_offset - gAveLine[1]) / getSlope(gAveLine) + gAveLine[0]);
		int yIntercept = int(gAveLine[1] - getSlope(gAveLine)*gAveLine[0]);
		gTopPt = Point(xTop, y_offset);
		gBotPt = (yIntercept < out.rows) ? Point(0, int(yIntercept)) : Point(int((out.rows - gAveLine[1]) / getSlope(gAveLine) + gAveLine[0]), out.rows);
		gTopDeq.emplace_front(gTopPt);
		gBotDeq.emplace_front(gBotPt);
		if (gTopDeq.size() > 3) {
			gTopDeq.pop_back();
			gBotDeq.pop_back();
		}
	}
	if (reds.size() == 0) {
		if (rTopDeq.at(0) != Point(0, 0)) {
			rTopDeq.emplace_front(rTopDeq.at(0));
			rBotDeq.emplace_front(rBotDeq.at(0));
		}
		rAveLine = Vec4i(0, 0, 0, 0);
	}
	else {
		rAveLine = std::accumulate(reds.begin(), reds.end(), Vec4i(0, 0, 0, 0)) / int(reds.size());
		//red top/bottom pts and line
		double xTop = (y_offset - rAveLine[1]) / getSlope(rAveLine) + rAveLine[0];
		double yIntercept = getSlope(rAveLine)*(out.cols - rAveLine[0]) + rAveLine[1];
		rTopPt = Point(int(xTop), y_offset);
		rBotPt = (yIntercept < out.rows) ? Point(out.cols, int(yIntercept)) : Point(int((out.rows - rAveLine[1]) / getSlope(rAveLine) + rAveLine[0]), out.rows);
		rBotDeq.emplace_front(rBotPt);
		rTopDeq.emplace_front(rTopPt);

		if (greens.size() > 0) //if both are > 0
			angleSums.emplace_front(getAngle(rAveLine) + getAngle(gAveLine));
	}
}
void Alg::drawLines(Mat& out, vector<Vec4i> greens, vector<Vec4i> reds) {
	if (gAveLine != Vec4i(0, 0, 0, 0)) {
		line(out, Point(gAveLine[0], gAveLine[1]), Point(gAveLine[2], gAveLine[3]), Color::green(), 1, 8);
		boxWrite(out, decStr(getAngle(gAveLine)), Point(10, 42));
		line(out, gTopPt, gBotPt, Color::magenta(), 1, 8);
		circle(out, gBotPt, 4, Color::magenta(), -1, 8);
		circle(out, gTopPt, 4, Color::magenta(), -1, 8);
	}
	
	if (rAveLine != Vec4i(0,0,0,0)) {
		line(out, Point(rAveLine[0], rAveLine[1]), Point(rAveLine[2], rAveLine[3]), Color::red(), 1, 8);
		boxWrite(out, decStr(getAngle(rAveLine)), Point(300, 42));
		line(out, rTopPt, rBotPt, Color::magenta(), 1, 8);
		circle(out, rBotPt, 4, Color::magenta(), -1, 8);
		circle(out, rTopPt, 4, Color::magenta(), -1, 8);
	}
	if (angleSums.size() == 9) {
		double angleAve = std::accumulate(angleSums.begin(), angleSums.end(), 0.0) / 9;
		boxWrite(out, decStr(angleAve), Point(170, 42));
		angleSums.pop_back();
		if (abs(angleAve) > 5)
			drawWarnArrows(out, angleAve);
	}
}
void Alg::drawMarks(Mat& out) {
	y_offset = int(offsetFactor * out.rows);
	Point yelPts[9] = { topMidP = Point(int(.5*out.cols), y_offset),
		topLeftP = Point(int(.23*out.cols), y_offset),		topMidLeftP = Point(int(.45*out.cols), y_offset),
		topMidRightP = Point(int(.55*out.cols), y_offset),	topRightP = Point(int(.77*out.cols), y_offset),
		botLeftP = Point(int(.25*out.cols), out.rows),		botRightP = Point(out.cols - int(.25*out.cols), out.rows),
		leftSideP = Point(0, int(.34*out.rows)),			rightSideP = Point(out.cols, int(.34*out.rows))
	};
	for (int i = 0; i < 9; i++)
		circle(out, yelPts[i], 3, Color::yellow(), -1, 8);
}
Mat Alg::init(Mat src, Mat& out) {
	resize(src, src, Size(int(resizeFactor*src.size().width), int(resizeFactor*src.size().height)));
	out = src.clone();
	Mat src_gray;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	return src_gray;
	Mat tmp = Mat(src.size(), COLOR_GRAY2BGR);
}
