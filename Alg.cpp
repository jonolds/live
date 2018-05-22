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
	inSmall = init(inFrame);						//Set values, resize inFrame, get grayImg
	grayImg = col2gr(inSmall);
	blurImg = superBlur();
	cannyImg = canny();
	maskDispImg = mask(gr2col(cannyImg), yellow);
	cannyMaskedImg = mask(cannyImg, black);
	houghImg = hough(allLines);
	sortLines(allLines);
	outFrame = drawLaneLines(gTop, rTop);
	showImages();
	return outFrame;
}

Mat Alg::superBlur() {
	Mat blurMat = grayImg.clone();
	blur(blurMat, blurMat, Size(5, 5));
	GaussianBlur(blurMat, blurMat, Size(7, 7), 4);
	GaussianBlur(blurMat, blurMat, Size(5, 7), 4);
	return blurMat;
}

Mat Alg::canny() {
	Mat cannyMat;
	Canny(blurImg, cannyMat, lowThr, highThr, 3, false);
	return cannyMat;
}

Mat Alg::mask(Mat img, Scalar color) {
	Mat maskMat = img.clone();
	rectangle(maskMat, Point(0, y_offset), Point(cols, 0), color, -1);
	Point triangle[3] = { Point(int(.1*cols), rows), Point(int(.9*cols), rows), Point(int(.5*cols), y_offset) };
	fillConvexPoly(maskMat, triangle, 3, color);
	return maskMat;
}

Mat Alg::hough(vector<Vec4i> & lines) {
	HoughLinesP(cannyMaskedImg, lines, 1, 1 * CV_PI / 180, hThresh, minLen, maxGap); //1st best{30, 20, 20}>{30,10,20}>{40, 20, 10}
	Mat houghMat = inSmall.clone();
	linez(houghMat, lines, green);
	return houghMat;
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
Mat Alg::drawLaneLines(LaneLine *lane, LaneLine *lane2) {
	Mat outMat = inSmall.clone();
	drawLaneLines(outMat, lane);
	drawLaneLines(outMat, lane2);
	drawMarks(outMat);
	return outMat;
}

void Alg::drawLaneLines(Mat& outMat, LaneLine *lane) {
	linez(outMat, lane->aveFrLine, red);
	boxWrite(outMat, decStr(getAngle(lane->aveFrLine)), (lane->color) ? Point(10, 42) : Point(300, 42));
	cirLine(outMat, pt2vec(lane->topPt, lane->botPt), magenta);
	if (angleSumsDeq.size() == 9) {
		double angleAve = std::accumulate(angleSumsDeq.begin(), angleSumsDeq.end(), 0.0) / 9;
		boxWrite(outMat, decStr(angleAve), Point(170, 42));
		angleSumsDeq.pop_back();
		if (abs(angleAve) > 5)
			drawWarnArrows(outMat, angleAve);
	}
}

void Alg::drawMarks(Mat& outMat) {
	linez(outMat, Vec4i(0, y_offset, cols, y_offset), pink);
	vector<Point> yelPts = { topMidPt = Point(int(.5*cols), y_offset),
		topLPt = Point(int(.23*cols), y_offset),			topMidLPt = Point(int(.45*cols), y_offset),
		topMidRPt = Point(int(.55*cols), y_offset),		topRightPt = Point(int(.77*cols), y_offset),
		botLPt = Point(int(.25*cols), rows),		botRPt = Point(cols - int(.25*cols), rows),
		sideLPt = Point(0, int(.34*rows)),					sideRPt = Point(cols, int(.34*rows)) };
	circz(outMat, yelPts, yellow);
}

Mat Alg::init(Mat inFrame) {
	Mat inReSz = reSz(inFrame, .35);
	rows = inReSz.rows; cols = inReSz.cols;
	y_offset = int(offsetFactor * rows);			//y_offset based on inSmall
	return inReSz;
}
Alg::Alg() {
	gTop = new LaneLine(GREEN, this), rTop = new LaneLine(RED, this);
	lowThr = 30, highThr = 105;						//Canny
	hThresh = 15, minLen = 20, maxGap = 70;			//Hough
}