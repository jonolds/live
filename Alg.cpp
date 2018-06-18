#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "Alg.h"
#include "helper.h"
#include "colors.h"
#include "Sector.h"
#include "draw.h"
using namespace std;
using namespace cv;

Mat Alg::process(Mat inFrame) {
	init(inFrame);										//0 inSmall
	grayImg = cvtGr(inSmall.clone());					//1 grayImg
	
	Mat blur2top = grayImg.clone().rowRange(0, int(grayImg.rows/2.8));
	Mat blur2bot = grayImg.clone().rowRange(int(grayImg.rows/2.8), grayImg.rows);
	Mat blur2 = grayImg.clone();

	//fastNlMeansDenoising(blur2top, blur2top, 12, 7, 21);
	//fastNlMeansDenoising(blur2bot, blur2bot, 16, 9, 21);

	
//	vconcat(blur2top, blur2bot, blur2);
	fastNlMeansDenoising(blur2, blur2, 35, 7, 15);
	vector<Vec4i> testVec;
	
	adaptiveThreshold(blur2, blur2, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 21, 4.5);
	show(blur2, 1, "thresh");
	HoughLinesP(blur2, testVec, 1, CV_PI/180, 10, 0, 50);
	cout << testVec.size() << "\n";
	for(Vec4i v : testVec)
		line(inSmall, Point(v[0], v[1]), Point(v[2], v[3]), white, 1, 8);
	

//	show(grayImg, 1, "grayImg");
//	show(blur2top, 1, "blur2top");
//	show(blur2bot, 1, "blur2bot");
//	show(blur2, 0, "blur2");


//	blur2 = superBlur(blur2);


//	blurImg = superBlur(grayImg.clone());				//2 blurImg
	show(inSmall, 1, "inSmall");
//	show(blurImg, 1, "blurImg");
	show(blur2, 1, "blur2");
	blurImg = blur2;


	canImg = canny(blurImg.clone(), lowThr, highThr);	//3 canImg
	maskGeneric = createGenericMask(canImg.clone());
	//mskAll(canImg.clone());								//4 mask
	createSectors();
	//houghImg = getHough(inSmall.clone());				//6 houghImg
	//outFrm = getOutFrame(inSmall.clone());				//7 outFrm			
	//show(outFrm);
	//showImages();
	outFrm = inSmall.clone();
	//show(outFrm);
	return cleanAndReturn();
}
void Alg::init(Mat inFrame) {
	inSmall = reSz(inFrame, .35);
	rows = inSmall.rows;			cols = inSmall.cols;		yOff = int(offsetFactor*rows);
	rhombus = { Pt(int(.1*cols), rows), Pt(int(.4*cols), yOff),
		Pt(int(.6*cols), yOff), Pt(int(.9*cols), rows) };
}
void Alg::mskAll(Mat mskIn) {
	Pt topPt1 = Pt(0, 85), topPt2 = Pt(cols, yOff), botPt1 = Pt(0, rows), botPt2 = Pt(cols, int(.5*rows));
	maskAll = createMask(mskIn, black);
	maskTop = createMask(mskIn, black, topPt1, topPt2);		
	maskBot = createMask(mskIn, black, botPt1, botPt2);
}
Mat Alg::createMask(Mat cnImg, Scalar color, Pt p1, Pt p2) {
	if (color != black)
		cnImg = cvtCol(cnImg);
	cnImg = rectangleRet(cnImg, Pt(0, yOff), Pt(cols, 0), color);
	fillConvexPoly(cnImg, &rhombus[0], 4, color);
	return rectangleRet(cnImg, p1, p2, color);
}
Mat Alg::createGenericMask(Mat cnImg) {
	cnImg = rectangleRet(cnImg, Pt(0, yOff), Pt(cols, 0), black);
	fillConvexPoly(cnImg, &rhombus[0], 4, black);
	return cnImg;
}

void Alg::createSectors() {
	sectors.emplace_back(Pt(int(.2*rows), int(.5*rows)), 1, maskGeneric);
	sectors.emplace_back(Pt(int(.3*rows), int(.6*rows)), 2, maskGeneric);
	sectors.emplace_back(Pt(int(.35*rows), rows), 3, maskGeneric);
}
//Mat Alg::getHough(Mat houghInputImg) {
//	allLns = HoughLinesP_t7vec(maskAll, .3, CV_PI/720, hThresh, minLen, maxGap);
//	allLnsTop = HoughLinesP_t7vec(maskTop, .3, CV_PI/720, hThresh, minLen, maxGap);
//	allLnsBot = HoughLinesP_t7vec(maskBot, .3, CV_PI/720, hThresh, minLen, maxGap);
//	sortHoughLines(allLns, rightLns, leftLns, badLns);
//	sortHoughLines(allLnsTop, rightLnsTop, leftLnsTop, badLnsTop);
//	sortHoughLines(allLnsBot, rightLnsBot, leftLnsBot, badLnsBot);
//	return drawHoughLines(houghInputImg, leftLnsBot, rightLnsBot, badLns);
//}

//Mat Alg::getOutFrame(Mat img) {
//	rightLns.drawAvLn(img);		leftLns.drawAvLn(img);
//	rightLnsBot.drawAvLn(img);	leftLnsBot.drawAvLn(img);
//	boxWrite(img, decStr(rightLns.avLn.ang), Pt(300, 42));
//	boxWrite(img, decStr(leftLns.avLn.ang), Pt(10, 42));
//	drawMarks(img);
//	return img;
//}

void Alg::paintAll(vector<Sector> secs) {
	for (Sector& s : secs)
		s.paintLines(outFrm);
}

Mat Alg::cleanAndReturn() {
	//for (Sector* t : t7vecsVec) t->clear();
	return outFrm;
}

void Alg::drawMarks(Mat& outMat) {
	ln(outMat, t7(0, yOff, cols, yOff), pink);
	keyPts = {
		topMidPt = Pt(cols/2, yOff),
		topLPt = Pt(cols*23/100, yOff), topMidLPt = Pt(cols*45/100, yOff),
		topMidRPt = Pt(cols*55/100, yOff), topRightPt = Pt(cols*77/100, yOff),
		botLPt = Pt(cols*25/100, rows), botRPt = Pt(cols - cols*25/100, rows),
		sideLPt = Pt(0, rows*34/100), sideRPt = Pt(cols, rows*34/100)
	};
	for (const Pt& p : keyPts) 
		dot(outMat, p, yellow);
}
void Alg::showImages() {
	vector<Mat> m = { inSmall, grayImg, blurImg, canImg, maskImg, maskGeneric, houghImg, outFrm };
	string labels[] = { "inSmall", "gray", "blur", "canImg", "maskImg", "maskGeneric", "getHough", "outFrm" };
	label(m, labels);
//	Mat vanImg = getVanImg(outFrm, leftLns.avLn, rightLns.avLn, rightLns.avLn.intrPt(leftLns.avLn));
	Mat dispImg = catRows(catCols(m[0], m[1], m[2]), catCols(m[3], m[4], m[5]), catCols(m[6], m[7], getSolidImg(m[7], gray)));
	imshow("views", reSz(dispImg, .8));
	waitKey(1);
}