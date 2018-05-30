#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include "Alg.h"
#include "helper.hpp"

using namespace std;
using namespace cv;

Mat Alg::process(Mat inFrame) {
	init(inFrame);										//0 inSmall
	grayImg = cvtColGr(inSmall);						//1 grayImg
	blurImg = getBlur(grayImg);							//2 blurImg
	canImg = getCanny(blurImg);							//3	canImg
	maskImg = getMask(cvtColGr(canImg), yellow);		//4	maskImg
	canMaskImg = getMask(cvtColGr(maskImg), black);	//5	canMaskImg
	houghImg = getHough();								//6 houghImg
	outFrm = getOutFrame();								//7 outFrm			
	showImages();
	return cleanAndReturn();
}
void Alg::init(Mat inFrame) {
	inSmall = reSz(inFrame, .35); houghImg = inSmall.clone(); outFrm = inSmall.clone();
	rows = inSmall.rows; cols = inSmall.cols; yOff = int(offsetFactor * rows);
}
Mat Alg::getBlur(Mat grImg) {
	return superBlur(grImg);
}
Mat Alg::getCanny(Mat blrImg) {
	return canny(blrImg, lowThr, highThr);
}
Mat Alg::getMask(Mat cnImg, Scalar color) {
	return mask(cnImg, color, yOff, rows, cols);
}
Mat Alg::getHough() {
	vVec4i tmp;
	HoughLinesP(canMaskImg, tmp, 1, CV_PI / 180, hThresh, minLen, maxGap);
	for(const Vec4i& t : tmp)
		allLns.emplace_back(t7i(t));
	sortHoughLines(*this);
	return drawHoughLines(inSmall.clone(), gLns, rLns, badLns, 0);
}
Mat Alg::getOutFrame() {
	Mat outMat = inSmall.clone();
	rAve = getAvLn(rLns); gAve = getAvLn(gLns);
	drawLaneLine(outMat, gAve); drawLaneLine(outMat, rAve);
	putText(outMat, to_string(rAve.vanX(gAve) - int(outMat.cols / 2)), Point(int(outMat.cols / 2), int(outMat.rows / 2)), 0, 1, white, 1);
	drawMarks(outMat);
	return outMat;
}

void Alg::drawLaneLine(Mat& outMat, t7i t) {
	lines(outMat, t.asVec(), (t.set == 'g') ? green : red, 2);
	boxWrite(outMat, decStr(t.getAng()), (t.set == 'g') ? Point(10, 42) : Point(300, 42));
	dotLns(outMat, t.asVec(), magenta);
	if (angleSumsDeq.size() == 9) {
		double angleAve = std::accumulate(angleSumsDeq.begin(), angleSumsDeq.end(), 0.0) / 9;
		boxWrite(outMat, decStr(angleAve), Point(170, 42));
		angleSumsDeq.pop_back();
		if (abs(angleAve) > 5)
			drawArrow(outMat, angleAve < 0);
	}
}
void Alg::drawMarks(Mat& outMat) {
	lines(outMat, t7i(0, yOff, cols, yOff).asVec(), pink);
	vector<Point> yelPts = { topMidPt = Point(int(.5*cols), yOff),
		topLPt = Point(int(.23*cols), yOff),		topMidLPt = Point(int(.45*cols), yOff),
		topMidRPt = Point(int(.55*cols), yOff),		topRightPt = Point(int(.77*cols), yOff),
		botLPt = Point(int(.25*cols), rows),			botRPt = Point(cols - int(.25*cols), rows),
		sideLPt = Point(0, int(.34*rows)),				sideRPt = Point(cols, int(.34*rows)) };
	for(const Point& p : yelPts) dot(outMat, p, yellow);
}
void Alg::showImages() {
	vector<Mat> m = { inSmall, grayImg, blurImg, canImg, maskImg, canMaskImg, houghImg, outFrm };
	string labels[] = { "inSmall", "gray", "blur", "canImg", "maskImg", "canMaskImg", "getHough", "outFrm" };
	label(m, labels);
	Mat vanImg = getVanImg(outFrm, gAve, rAve, rAve.intrPt(gAve));
	//Mat dispImg = catRows(catCols(m[3], m[4], m[5]), catCols(m[6], m[7], getSolidImg(m[7], gray)), vanImg);
	Mat dispImg = catRows(catCols(m[0], m[1], m[2]), catCols(m[3], m[4], m[5]), catCols(m[6], m[7], getSolidImg(m[7], gray)));
	imshow("views", reSz(dispImg, .8)); waitKey();
}

Mat Alg::cleanAndReturn() {
	allLns.clear(); gLns.clear(); rLns.clear(); badLns.clear();
	return outFrm;
}
Alg::Alg() {
}