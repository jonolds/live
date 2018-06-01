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

	grayImg = cvtGr(inSmall.clone());					//1 grayImg
	blurImg = superBlur(grayImg.clone());				//2 blurImg
	canImg = canny(blurImg.clone(), lowThr, highThr);	//3 canImg
	mskAll(canImg.clone());								//4 mask
	houghImg = getHough(mskFullBl, grid);							//6 houghImg
	outFrm = getOutFrame(inSmall.clone());				//7 outFrm			
	//showImages();
	return cleanAndReturn();
}
void Alg::init(Mat inFrame) {
	inSmall = reSz(inFrame, .35);
	houghImg = inSmall.clone();		outFrm = inSmall.clone();
	rows = inSmall.rows;			cols = inSmall.cols;
	yOff = int(offsetFactor * rows);
}
void Alg::mskAll(Mat mskIn) {
	mskFullBl = getMskInit(mskIn, black);
	mskHghBl = getMskHgh(mskIn, black);		mskLwBl = getMskLw(mskIn, black);
	mskFullYel = getMskInit(mskIn, yellow);
	mskHghYel = getMskHgh(mskIn, yellow);	mskLwYel = getMskLw(mskIn, yellow);
	
	Mat group = cat2by3(mskFullBl, mskFullYel, mskLwBl, mskLwYel, mskHghBl, mskHghYel);
	show(group);
}
Mat Alg::getMskHgh(Mat cnImg, Scalar color) {
	cnImg = getMskInit(cnImg, color);
	return rectangleRet(cnImg, Pt(0, 85), Pt(cols, yOff), color);
}
Mat Alg::getMskLw(Mat cnImg, Scalar color) {
	cnImg = getMskInit(cnImg, color);
	return rectangleRet(cnImg, Pt(0, rows), Pt(cols, 150), color);
}
Mat Alg::getMskInit(Mat cnImg, Scalar color) {
	Pt rhombus[4] = { Pt(int(.1 * cols), rows), Pt(int(.4 * cols), yOff),
		Pt(int(.6 * cols), yOff), Pt(int(.9 * cols), rows) };
	if (color != black)
		cnImg = cvtCol(cnImg);
	cnImg = rectangleRet(cnImg, Pt(0, yOff), Pt(cols, 0), color);
	fillConvexPoly(cnImg, rhombus, 4, color);
	return cnImg;
}
Mat Alg::getHough(Mat input, Mat& output) {
	vVec4i tmp;
	HoughLinesP(input, tmp, 1, CV_PI / 180, hThresh, minLen, maxGap);
	for (const Vec4i& t : tmp)
		allLns.emplace_back(t7(t));
	sortHoughLines(*this);
	return drawHoughLines(inSmall.clone(), grLnsBot, rLnsBot, badLns);
}
void Alg::sortHoughLines(Alg& alg) {
	for (t7& t : alg.allLns) {
		if ((t.ang > 15) && (t.ang < 65) && (t[0] > alg.topMidRPt.x) && (t[2] > alg.topMidRPt.x))
			alg.rLnsBot.emplace_back(t);
		else if ((t.ang < 15) && (t.ang < 65) && (t[0] < alg.topMidLPt.x) && (t[2] < alg.topMidLPt.x))
			alg.grLnsBot.emplace_back(t);
		else
			alg.badLns.emplace_back(t);
	}
}
Mat Alg::getOutFrame(Mat img) {
	rAve = rLnsBot.drwAv(img);
	grAve = grLnsBot.drwAv(img);
	putText(img, to_string(rAve.vanX(grAve) - int(img.cols / 2)), Pt(int(img.cols / 2), int(img.rows / 2)), 0, 1, white, 1);
	boxWrite(img, decStr(rAve.ang), Pt(300, 42));
	boxWrite(img, decStr(rAve.ang), Pt(10, 42));
	drawMarks(img);
	drawArrow(img, true);
	return img;
}
void Alg::drawMarks(Mat& outMat) {
	ln(outMat, t7(0, yOff, cols, yOff), pink);
	vector<Pt> yelPts = {
		topMidPt = Pt(int(.5 * cols), yOff),
		topLPt = Pt(int(.23 * cols), yOff), topMidLPt = Pt(int(.45 * cols), yOff),
		topMidRPt = Pt(int(.55 * cols), yOff), topRightPt = Pt(int(.77 * cols), yOff),
		botLPt = Pt(int(.25 * cols), rows), botRPt = Pt(cols - int(.25 * cols), rows),
		sideLPt = Pt(0, int(.34 * rows)), sideRPt = Pt(cols, int(.34 * rows))
	};
	for (const Pt& p : yelPts) dot(outMat, p, yellow);
}
void Alg::showImages() {
	vector<Mat> m = {inSmall, grayImg, blurImg, canImg, maskImg, cnMskImg, houghImg, outFrm};
	string labels[] = {"inSmall", "gray", "blur", "canImg", "maskImg", "cnMskImg", "getHough", "outFrm"};
	label(m, labels);
	Mat vanImg = getVanImg(outFrm, grAve, rAve, rAve.intrPt(grAve));
	//Mat dispImg = catRows(catCols(m[3], m[4], m[5]), catCols(m[6], m[7], getSolidImg(m[7], gray)), vanImg);
	Mat dispImg = catRows(catCols(m[0], m[1], m[2]), catCols(m[3], m[4], m[5]), catCols(m[6], m[7], getSolidImg(m[7], gray)));
	imshow("views", reSz(dispImg, .8));
	waitKey(1);
}
Mat Alg::cleanAndReturn() {
	allLns.clear();
	grLnsBot.clear();
	rLnsBot.clear();
	badLns.clear();
	return outFrm;
}
Alg::Alg() {
	allLns = t7vec(brown), grLnsBot = t7vec(green), rLnsBot = t7vec(red), badLns = t7vec(white);
}