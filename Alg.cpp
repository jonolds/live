#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "Alg.h"
#include "helper.h"
#include "basics.h"
#include "colors.h"
#include "t7vec.h"
#include "draw.h"

using namespace std;
using namespace cv;

Mat Alg::process(Mat inFrame) {
	init(inFrame); //0 inSmall
	vector<Mat> gp = makeGrid(inSmall.clone(), yOff);
	side = gp[1]; grid = gp[0];

	grayImg = cvtGr(inSmall.clone()); //1 grayImg
	blurImg = getBlur(grayImg.clone()); //2 blurImg
	canImg = getCanny(blurImg.clone()); //3	canImg

	mskSplit(canImg.clone());

	houghImg = getHough(); //6 houghImg
	outFrm = getOutFrame(inSmall.clone()); //7 outFrm			
	//showImages();
	//hconcat(side, grid, grid); imshow("grid", grid); waitKey();
	//showImages();
	return cleanAndReturn();
}
void Alg::mskSplit(Mat mskIn) {
	Point rhombus[4] = { Point(int(.1 * cols), rows), Point(int(.4 * cols), yOff),
		Point(int(.6 * cols), yOff), Point(int(.9 * cols), rows) };
	
	//mask offset/rhombus area
	Mat mskInitYel = rectangleRet(cvtCol(mskIn.clone()), Point(0, yOff), Point(cols, 0), yellow);
	fillConvexPoly(mskInitYel, rhombus, 4, yellow);
	Mat mskInitBl = rectangleRet(mskIn.clone(), Point(0, yOff), Point(cols, 0), black);
	fillConvexPoly(mskInitBl, rhombus, 4, black);

	//mask high area
	Mat mskHghYel = rectangleRet(mskInitYel, Point(0, 85), Point(cols, yOff), yellow);
	mskHghBl = rectangleRet(mskInitBl, Point(0, 85), Point(cols, yOff), black);

	//mask low area
	Mat mskLwYel = rectangleRet(mskInitYel, Point(0, rows), Point(cols, 150), yellow);
	mskLwBl = rectangleRet(mskInitBl, Point(0, rows), Point(cols, 150), black);

	mskHghYel = catCols(mskLwBl, mskHghBl, mskLwBl);
	vconcat(mskHghYel, cvtCol(mskHghBl), mskHghYel);
	imshow("grid", mskHghYel); waitKey();
}
Mat Alg::getMskHgh(Mat cnImg, Scalar color, int yLow) {
	rectangle(cnImg, Point(0, yOff), Point(cols, 0), color, -1);
	rectangle(cnImg, Point(0, rows), Point(cols, yLow), color, -1);
	Point rhombus[4] = { Point(int(.1 * cols), rows), Point(int(.4 * cols), yOff), Point(int(.6 * cols), yOff), Point(int(.9 * cols), rows) };
	fillConvexPoly(cnImg, rhombus, 4, color);
	return cnImg; 
}
Mat Alg::getMskLw(Mat cnImg, Scalar color, int yLow) {
	rectangle(cnImg, Point(0, yLow), Point(cols, 0), color, -1);
	Point rhombus[4] = { Point(int(.1 * cols), rows), Point(int(.4 * cols), yOff), Point(int(.6 * cols), yOff), Point(int(.9 * cols), rows) };
	fillConvexPoly(cnImg, rhombus, 4, color);
	return cnImg;
}
Mat maskTop(Mat mask, Scalar color, int y_offset, int rows, int cols) {
	rectangle(mask, Point(0, y_offset), Point(cols, 0), color, -1);
	Point rhombus[4] = { Point(int(.1 * cols), rows), Point(int(.4 * cols), y_offset), Point(int(.6 * cols), y_offset), Point(int(.9 * cols), rows) };
	fillConvexPoly(mask, rhombus, 4, color);
	return mask;
}

Mat Alg::getHough() {
	vVec4i tmp;
	HoughLinesP(cnMskImg, tmp, 1, CV_PI / 180, hThresh, minLen, maxGap);
	for (const Vec4i& t : tmp)
		allLns.emplace_back(t7(t));
	sortHoughLines(*this);
	return drawHoughLines(inSmall.clone(), gLns, rLns, badLns);
}
void Alg::sortHoughLines(Alg& alg) {
	for (t7& t : alg.allLns) {
		if ((t.ang > 15) && (t.ang < 65) && (t[0] > alg.topMidRPt.x) && (t[2] > alg.topMidRPt.x))
			alg.rLns.emplace_back(t);
		else if ((t.ang < 15) && (t.ang < 65) && (t[0] < alg.topMidLPt.x) && (t[2] < alg.topMidLPt.x))
			alg.gLns.emplace_back(t);
		else
			alg.badLns.emplace_back(t);
	}
}
void Alg::init(Mat inFrame) {
	inSmall = reSz(inFrame, .35);
	houghImg = inSmall.clone();
	outFrm = inSmall.clone();
	rows = inSmall.rows;
	cols = inSmall.cols;
	yOff = int(offsetFactor * rows);
}

Mat Alg::getBlur(Mat grImg) {
	return superBlur(grImg);
}

Mat Alg::getCanny(Mat blrImg) {
	return canny(blrImg, lowThr, highThr);
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
	vector<Point> yelPts = {
		topMidPt = Point(int(.5 * cols), yOff),
		topLPt = Point(int(.23 * cols), yOff), topMidLPt = Point(int(.45 * cols), yOff),
		topMidRPt = Point(int(.55 * cols), yOff), topRightPt = Point(int(.77 * cols), yOff),
		botLPt = Point(int(.25 * cols), rows), botRPt = Point(cols - int(.25 * cols), rows),
		sideLPt = Point(0, int(.34 * rows)), sideRPt = Point(cols, int(.34 * rows))
	};
	for (const Point& p : yelPts) dot(outMat, p, yellow);
}

void Alg::showImages() {
	vector<Mat> m = {inSmall, grayImg, blurImg, canImg, maskImg, cnMskImg, houghImg, outFrm};
	string labels[] = {"inSmall", "gray", "blur", "canImg", "maskImg", "cnMskImg", "getHough", "outFrm"};
	label(m, labels);
	Mat vanImg = getVanImg(outFrm, gAve, rAve, rAve.intrPt(gAve));
	//Mat dispImg = catRows(catCols(m[3], m[4], m[5]), catCols(m[6], m[7], getSolidImg(m[7], gray)), vanImg);
	Mat dispImg = catRows(catCols(m[0], m[1], m[2]), catCols(m[3], m[4], m[5]), catCols(m[6], m[7], getSolidImg(m[7], gray)));
	imshow("views", reSz(dispImg, .8));
	waitKey(1);
}

Mat Alg::cleanAndReturn() {
	allLns.clear();
	gLns.clear();
	rLns.clear();
	badLns.clear();
	return outFrm;
}

Alg::Alg() {
	allLns = t7vec(brown), gLns = t7vec(green), rLns = t7vec(red), badLns = t7vec(white);
}
