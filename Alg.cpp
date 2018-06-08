#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "Alg.h"
#include "helper.h"
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
	houghImg = getHough();								//6 houghImg
	outFrm = getOutFrame(inSmall.clone());				//7 outFrm			
	show(outFrm);
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
	Pt hghMskPt1 = Pt(0, 85), hghMskPt2 = Pt(cols, yOff), lwMskPt1 = Pt(0, rows), lwMskPt2 = Pt(cols, int(.5*rows));
	mskFullBl = getMsk(mskIn, black);

	mskHghBl = getMsk(mskIn, black, hghMskPt1, hghMskPt2);		
	mskLwBl = getMsk(mskIn, black, lwMskPt1, lwMskPt2);
	mskFullYel = getMsk(mskIn, yellow);
	mskHghYel = getMsk(mskIn, yellow, hghMskPt1, hghMskPt2);
	mskLwYel = getMsk(mskIn, yellow, lwMskPt1, lwMskPt2);
	
	/*Mat group = cat2by3(mskFullBl, mskFullYel, mskLwBl, mskLwYel, mskHghBl, mskHghYel);
	show(group);*/
}
Mat Alg::getMsk(Mat cnImg, Scalar color, Pt p1, Pt p2) {
	Pt rhombus[4] = { Pt(int(.1 * cols), rows), Pt(int(.4 * cols), yOff),
		Pt(int(.6 * cols), yOff), Pt(int(.9 * cols), rows) };
	if (color != black)
		cnImg = cvtCol(cnImg);
	cnImg = rectangleRet(cnImg, Pt(0, yOff), Pt(cols, 0), color);
	fillConvexPoly(cnImg, rhombus, 4, color);
	return rectangleRet(cnImg, p1, p2, color);
}
Mat Alg::getHough() {
	vVec4i tmp1, tmp2, tmp3;
	HoughLinesP(mskFullBl, tmp1, .3, CV_PI / 720, hThresh, minLen, maxGap);
	HoughLinesP(mskHghBl, tmp2, .3, CV_PI / 720, hThresh, minLen, maxGap);
	HoughLinesP(mskLwBl, tmp3, .3, CV_PI / 720, hThresh, minLen, maxGap);
	for (const Vec4i& t : tmp1)
		allLns.emplace_back(t7(t));
	for (const Vec4i& t : tmp2)
		allLnsTop.emplace_back(t7(t));
	for (const Vec4i& t : tmp3)
		allLnsBot.emplace_back(t7(t));
	sortHoughLines(allLns, rLns, grLns, badLns);
	sortHoughLines(allLnsTop, rLnsTop, grLnsTop, badLnsTop);
	sortHoughLines(allLnsBot, rLnsBot, grLnsBot, badLnsBot);
	return drawHoughLines(inSmall.clone(), grLnsBot, rLnsBot, badLns);
}
void Alg::sortHoughLines(t7vec& allVec,  t7vec& rVec, t7vec& gVec, t7vec& bVec) {
	for (t7& t : allVec) {
		if ((t.ang > 15) && (t.ang < 85) && (t[0] > topMidRPt.x) && (t[2] > topMidRPt.x))
			rVec.emplace_back(t);
		else if ((t.ang < -15) && (t.ang > -85) && (t[0] < topMidLPt.x) && (t[2] < topMidLPt.x))
			gVec.emplace_back(t);
		else
			bVec.emplace_back(t);
	}
}
Mat Alg::getOutFrame(Mat img) {
	rLnsBot.drawAvLn(img);
	grLnsBot.drawAvLn(img);
	rLns.drawAvLn(img);
	grLns.drawAvLn(img);
	//putText(img, to_string(rAve.vanX(grAve) - int(img.cols / 2)), Pt(int(img.cols / 2), int(img.rows / 2)), 0, 1, white, 1);
	boxWrite(img, decStr(rLns.avLn.ang), Pt(300, 42));
	boxWrite(img, decStr(grLns.avLn.ang), Pt(10, 42));
	drawMarks(img);
	//drawArrow(img, true);
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
	Mat vanImg = getVanImg(outFrm, grLns.avLn, rLns.avLn, rLns.avLn.intrPt(grLns.avLn));
	//Mat dispImg = catRows(catCols(m[3], m[4], m[5]), catCols(m[6], m[7], getSolidImg(m[7], gray)), vanImg);
	Mat dispImg = catRows(catCols(m[0], m[1], m[2]), catCols(m[3], m[4], m[5]), catCols(m[6], m[7], getSolidImg(m[7], gray)));
	imshow("views", reSz(dispImg, .8));
	waitKey(1);
}
Mat Alg::cleanAndReturn() {
	allLns.clear(); allLnsTop.clear(); allLnsBot.clear();
	grLns.clear();  grLnsTop.clear(); grLnsBot.clear();
	rLns.clear(); rLnsTop.clear(); rLnsBot.clear();
	badLns.clear(); badLnsTop.clear(); badLnsBot.clear();
	return outFrm;
}
Alg::Alg() {
	allLns = t7vec(brown), allLnsTop = t7vec(brown), allLnsBot = t7vec(brown);
	grLns = t7vec(green), grLnsTop = t7vec(yellow), grLnsBot = t7vec(orange);
	rLns = t7vec(red), rLnsTop = t7vec(yellow), rLnsBot = t7vec(orange);
	badLns = t7vec(white), badLnsTop = t7vec(white), badLnsBot = t7vec(white);
}