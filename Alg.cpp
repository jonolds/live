#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include "Alg.h"
#include "helper.hpp"
using namespace std; using namespace cv;

struct LaneLine;

Mat Alg::process(Mat inFrame) {
	vector<Vec4i> allLines;
	init(inFrame);						//Set values, resize inFrame, get grayImg
	canny();
	mask(black);
	hough(allLines);
	mask(yellow);
	sortLines(allLines);
	drawLaneLines(gTop, rTop);
	drawMarks();
	showImages();
	cleanup();
	return outFrame;
}

void Alg::canny() {
	blurImg = superBlur(grayImg);
	Canny(blurImg, edgeImg, lowThr, highThr, 3, false);
}

void Alg::mask(Scalar color) {
	maskImg = edgeImg.clone();
	rectangle(maskImg, Point(0, y_offset), Point(cols, 0), color, -1);
	Point triangle[3] = { Point(int(.1*cols), rows), Point(int(.9*cols), rows), Point(int(.5*cols), y_offset) };
	fillConvexPoly(maskImg, triangle, 3, color);
}

void Alg::hough(vector<Vec4i> & lines) {
	HoughLinesP(maskImg, lines, 1, 1 * CV_PI / 180, hThresh, minLen, maxGap); //1st best{30, 20, 20}>{30,10,20}>{40, 20, 10} 
	linez(houghImg, lines, green);
}

void Alg::sortLines(vector<Vec4i>& lines) {
	for (const Vec4i& line : lines) {
		double angle = getAngle(line);
		//Check if angle is between 20 and 62
		if ((abs(angle) > 20) && (abs(angle) < 62)) {
			//if angle is pos and both endpoint x values are greater than right midpoint add to reds
			if ((angle > 0) && (line[0] > topMidRPt.x) && (line[2] > topMidRPt.x))
				rTop->lines.emplace_back(line);
			//if angle is neg and both endpoint x values are less than right midpoint add to greens
			else if ((angle < 0) && (line[1] < topMidLPt.x) && (line[2] < topMidLPt.x))
				gTop->lines.emplace_back(line);
		}
	}
	//set aveFrLine for both lanes
	setLinePts(rows, cols, gTop, rTop);
	if ((!gTop->lines.empty()) && (!rTop->lines.empty())) //if both are > 0
		angleSumsDeq.emplace_front(getAngle(rTop->aveFrLine) + getAngle(gTop->aveFrLine));
}

void Alg::drawLaneLines(LaneLine *lane) {
	linez(outFrame, lane->aveFrLine, red);
	boxWrite(outFrame, decStr(getAngle(lane->aveFrLine)), (lane->color) ? Point(10, 42) : Point(300, 42));
	cirLine(outFrame, pt2vec(lane->topPt, lane->botPt), magenta);
	if (angleSumsDeq.size() == 9) {
		double angleAve = std::accumulate(angleSumsDeq.begin(), angleSumsDeq.end(), 0.0) / 9;
		boxWrite(outFrame, decStr(angleAve), Point(170, 42));
		angleSumsDeq.pop_back();
		if (abs(angleAve) > 5)
			drawWarnArrows(outFrame, angleAve);
	}
}

void Alg::drawMarks() {
	vector<Point> yelPts = { topMidPt = Point(int(.5*cols), y_offset),
		topLPt = Point(int(.23*cols), y_offset),			topMidLPt = Point(int(.45*cols), y_offset),
		topMidRPt = Point(int(.55*cols), y_offset),		topRightPt = Point(int(.77*cols), y_offset),
		botLPt = Point(int(.25*cols), rows),		botRPt = Point(cols - int(.25*cols), rows),
		sideLPt = Point(0, int(.34*rows)),					sideRPt = Point(cols, int(.34*rows)) };
	circz(outFrame, yelPts, yellow);
}

void Alg::init(Mat inFrame) {
	gTop = new LaneLine(GREEN, this), rTop = new LaneLine(RED, this);
	lowThr = 30, highThr = 105;						//Canny
	hThresh = 15, minLen = 20, maxGap = 70;			//Hough
	inSmall = reSz(inFrame, .35);					//get inSmall
	outFrame = inSmall.clone();						//init outFrame
	houghImg = inSmall.clone();						//init houghImg
	tmp = outFrame.clone();
	rows = outFrame.rows; cols = outFrame.cols;
	y_offset = int(offsetFactor * rows);			//y_offset based on inSmall
	cvtColor(inSmall, grayImg, COLOR_BGR2GRAY);		//get grayImg
	linez(outFrame, Vec4i(0, y_offset, cols, y_offset), pink);
}
Alg::Alg() {}