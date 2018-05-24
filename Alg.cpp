#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include "Alg.h"
#include "helper.hpp"
using namespace std; 
using namespace cv;

struct LaneLine;

Mat Alg::process(Mat inFrame) {
	vector<Vec4i> allLines;
	init(inFrame);
	cannyHough();
	outFrame = drawLaneLines(gTop, rTop);
	showImages();
	rTop.clear(); gTop.clear();
	//imshow("out", outFrame);
	//if(waitKey(700) > 0)
	//	waitKey();
	return outFrame;
}

void Alg::init(Mat inFrame) {
	gTop.set(GREEN, this); rTop.set(RED, this);
	inSmall = reSz(inFrame, .35); houghImg = inSmall.clone(); outFrame = inSmall.clone();
	rows = inSmall.rows; cols = inSmall.cols;
	y_offset = int(offsetFactor * rows);
}

void Alg::cannyHough() {
	grayImg = col2gr(inSmall);
	//Blur
	blur(grayImg.clone(), blurImg, Size(5, 5));
	GaussianBlur(blurImg, blurImg, Size(7, 7), 4);
	GaussianBlur(blurImg, blurImg, Size(5, 7), 4);

	//Canny
	lowThr = 20, highThr = 50;
	Canny(blurImg, cannyImg, lowThr, highThr);
	
	//Mask
	maskDispImg = mask(gr2col(cannyImg), yellow);
	cannyMaskedImg = mask(col2gr(maskDispImg), black);

	//Hough
	vector<Vec4i> lines;
	hThresh = 15, minLen = 20, maxGap = 70;			//Hough settings
	HoughLinesP(cannyMaskedImg, lines, 1, 1 * CV_PI / 180, hThresh, minLen, maxGap); //1st best{30, 20, 20}>{30,10,20}>{40, 20, 10}
	//linez(houghImg, lines, green);

	vector<Vec4i> badLines;
	//Sort into gTop/rTop lines
	for (const Vec4i& line : lines) {
		double angle = getAngle(line);
		if ((angle > 15) && (angle < 65) && (line[0] > topMidRPt.x) && (line[2] > topMidRPt.x))
			rTop.lines.emplace_back(line);
		else if ((angle < 15) && (angle < 65) && (line[0] < topMidLPt.x) && (line[2] < topMidLPt.x))
			gTop.lines.emplace_back(line);
		else
			badLines.push_back(line);
	}
	linez(houghImg, badLines, white);
	linez(houghImg, gTop.lines, green);
	linez(houghImg, rTop.lines, red);
	badLines.clear();



	//set aveFrLine for both lanes
	setLinePts(rows, cols, gTop, rTop);
	if ((!gTop.lines.empty()) && (!rTop.lines.empty())) //if both are > 0
		angleSumsDeq.emplace_front(getAngle(rTop.aveFrLine) + getAngle(gTop.aveFrLine));
}
Mat Alg::mask(Mat img, Scalar color) {
	rectangle(img, Point(0, y_offset), Point(cols, 0), color, -1);
	Point rhombus[4] = { Point(int(.1*cols), rows), Point(int(.4*cols), y_offset), Point(int(.6*cols), y_offset), Point(int(.9*cols), rows) };
	fillConvexPoly(img, rhombus, 4, color);
	return img;
}

Mat Alg::drawLaneLines(LaneLine lane, LaneLine lane2) {
	Mat outMat = inSmall.clone();
	drawLaneLines(outMat, lane);
	drawLaneLines(outMat, lane2);
	drawMarks(outMat);
	return outMat;
}
void Alg::drawLaneLines(Mat& outMat, LaneLine lane) {
	linez(outMat, lane.aveFrLine, red);
	boxWrite(outMat, decStr(getAngle(lane.aveFrLine)), (lane.color) ? Point(10, 42) : Point(300, 42));
	cirLine(outMat, pt2vec(lane.topPt, lane.botPt), magenta);
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
void Alg::showImages() {
	vector<Mat> m = { inSmall, grayImg, blurImg, cannyImg, maskDispImg, cannyMaskedImg, houghImg, outFrame };
	string labels[] = { "inSmall", "gray", "blur", "cannyImg", "maskDispImg", "cannyMaskedImg", "hough", "outFrame" };
	cvtColAddLabel(m, labels);
	Mat dispImg = concatCols(concatRow(m[0], m[1], m[2]), concatRow(m[3], m[4], m[5]), concatRow(m[6], m[7], grayMat(m[7])));
	namedWindow("views", WINDOW_KEEPRATIO);
	imshow("views", dispImg);
	if(waitKey(50) > 0)
		waitKey();
}