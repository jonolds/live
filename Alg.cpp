#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
#include <math.h>
#include <iomanip>
#include <numeric>
#include "Alg.h"
#include "helper.hpp"
struct LaneLine;
using namespace std; using namespace cv; using namespace viz;

void Alg::process(Mat& src, Mat& out) {
	/*Canny*/lowThr = 30, highThr = 105, /*Hough*/ hThresh = 15, minLen = 20, maxGap = 70;
	vector<Vec4i> lines;
	Mat src_gray = init(src, out);
	applyCannyHough(src_gray, out, lines);
	sortLines(out, lines);
	drawLaneLines(out, g);
	drawLaneLines(out, r);
	drawMarks(out);
}

void Alg::applyCannyHough(Mat src_gray, Mat& out, vector<Vec4i> & lines) {
	blur(src_gray, src_gray, Size(5, 5));
	GaussianBlur(src_gray, src_gray, Size(7, 7), 4);
	GaussianBlur(src_gray, src_gray, Size(5, 7), 4);
	Mat edges(src_gray.size(), CV_8U, Scalar::all(0));
	Canny(src_gray, edges, lowThr, highThr, 3, false);

	rectangle(edges, Point(0, y_offset), Point(src_gray.cols, 0), Scalar::all(0), -1);
	Point triangle[3] = { Point(int(.1*edges.cols), edges.rows), Point(int(.9*edges.cols), edges.rows), Point(int(.5*edges.cols), y_offset) };
	fillConvexPoly(edges, triangle, 3, Scalar::all(255));

	imshow("canny", edges);
	fillConvexPoly(edges, triangle, 3, Scalar::all(0));
	fillConvexPoly(out, triangle, 3, Scalar::all(255));//color white triangle on out
	
	HoughLinesP(edges, lines, 1, 1 * CV_PI / 180, hThresh, minLen, maxGap); //1st best{30, 20, 20}>{30,10,20}>{40, 20, 10} 
	line(out, Point(0, y_offset), Point(out.size().width, y_offset), Color::pink(), 1, 8);
}

void Alg::sortLines(Mat& out, vector<Vec4i>& lines) {
	for (size_t i = 0; i < lines.size(); i++) {
		int x1, y1, x2, y2;
		double angle = getAngle(lines[i], x1, y1, x2, y2);
		if ((abs(angle) > 20) && (abs(angle) < 62)) {
			if ((angle > 0) && (x1 > topMidRPt.x) && (x2 > topMidRPt.x))
				r.lines.emplace_back(lines[i]);
			else if ((angle < 0) && (x1 < topMidLPt.x) && (x2 < topMidLPt.x))
				g.lines.emplace_back(lines[i]);
		}
	}
	setLinePts(out, g);
	setLinePts(out, r);
	if ((g.lines.size() > 0) && (r.lines.size() > 0)) //if both are > 0
		angleSumsDeq.emplace_front(getAngle(r.aveLine) + getAngle(g.aveLine));
}

void Alg::drawLaneLines(Mat& out, LaneLine& lane) {
		line(out, Point(lane.aveLine[0], lane.aveLine[1]), Point(lane.aveLine[2], lane.aveLine[3]), Color::red(), 1, 8);
		boxWrite(out, decStr(getAngle(lane.aveLine)), (lane.color) ? Point(10, 42) : Point(300, 42));
		line(out, lane.topPt, lane.botPt, Color::magenta(), 1, 8);
		circle(out, lane.botPt, 4, Color::magenta(), -1, 8);
		circle(out, lane.topPt, 4, Color::magenta(), -1, 8);
	if (angleSumsDeq.size() == 9) {
		double angleAve = std::accumulate(angleSumsDeq.begin(), angleSumsDeq.end(), 0.0) / 9;
		boxWrite(out, decStr(angleAve), Point(170, 42));
		angleSumsDeq.pop_back();
		if (abs(angleAve) > 5)
			drawWarnArrows(out, angleAve);
	}
}

void Alg::drawMarks(Mat& out) {
	y_offset = int(offsetFactor * out.rows);
	Point yelPts[9] = { topMidPt = Point(int(.5*out.cols), y_offset),
		topLPt = Point(int(.23*out.cols), y_offset),		topMidLPt = Point(int(.45*out.cols), y_offset),
		topMidRPt = Point(int(.55*out.cols), y_offset),		topRightPt = Point(int(.77*out.cols), y_offset),
		botLPt = Point(int(.25*out.cols), out.rows),		botRPt = Point(out.cols - int(.25*out.cols), out.rows),
		sideLPt = Point(0, int(.34*out.rows)),				sideRPt = Point(out.cols, int(.34*out.rows)) };
	for (int i = 0; i < 9; i++)
		circle(out, yelPts[i], 3, Color::yellow(), -1, 8);
}

Mat Alg::init(Mat src, Mat& out) {
	y_offset = int(offsetFactor * out.rows);
	resize(src, src, Size(int(resizeFactor*src.size().width), int(resizeFactor*src.size().height)));
	out = src.clone();
	Mat src_gray;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	return src_gray;
}