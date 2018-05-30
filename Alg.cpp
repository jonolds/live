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
	grayImg = cvtColGr(inSmall.clone());						//1 grayImg
	blurImg = getBlur(grayImg.clone());							//2 blurImg
	canImg = getCanny(blurImg.clone());							//3	canImg
	maskImg = getMask(cvtColGr(canImg.clone()), yellow);		//4	maskImg
	canMaskImg = getMask(cvtColGr(maskImg.clone()), black);		//5	canMaskImg
	houghImg = getHough();								//6 houghImg
	outFrm = getOutFrame(inSmall.clone());								//7 outFrm			
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
		allLns.emplace_back(t7(t));
	sortHoughLines(*this);
	return drawHoughLines(inSmall.clone(), gLns, rLns, badLns);
}
inline void Alg::sortHoughLines(Alg& alg) {
	for (t7& t : alg.allLns) {
		if ((t.ang > 15) && (t.ang < 65) && (t[0] > alg.topMidRPt.x) && (t[2] > alg.topMidRPt.x))
			alg.rLns.emplace_back(t);
		else if ((t.ang < 15) && (t.ang < 65) && (t[0] < alg.topMidLPt.x) && (t[2] < alg.topMidLPt.x))
			alg.gLns.emplace_back(t);
		else
			alg.badLns.emplace_back(t);
	}
}
Mat Alg::getOutFrame(Mat img) {
	rAve = rLns.drwAv(img);
	gAve = gLns.drwAv(img);
	putText(img, to_string(rAve.vanX(gAve) - int(img.cols / 2)), Point(int(img.cols / 2), int(img.rows / 2)), 0, 1, white, 1);
	boxWrite(img, decStr(rAve.ang), Point(300, 42)); 
	boxWrite(img, decStr(rAve.ang), Point(10, 42));
	drawMarks(img);
	drawArrow(img, true);
	return img;
}

void Alg::drawMarks(Mat& outMat) {
	ln(outMat, t7(0, yOff, cols, yOff), pink);
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
	imshow("views", reSz(dispImg, .8)); waitKey(1);
}

Mat Alg::cleanAndReturn() {
	allLns.clear(); gLns.clear(); rLns.clear(); badLns.clear();
	return outFrm;
}
Alg::Alg() {
	allLns = t7vec(brown), gLns = t7vec(green), rLns = t7vec(red), badLns = t7vec(white);
}