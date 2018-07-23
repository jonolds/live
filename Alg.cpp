#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "Alg.h"
#include "helper.h"
#include "constants.h"
#include "Sector.h"
#include "draw.h"
using namespace std;
using namespace cv;

Alg::Alg() {
	//createTrackbar("LowH", "control", &LowH, 179); //Hue (0 - 179)
	//createTrackbar("HighH", "control", &HighH, 179);
	//createTrackbar("LowS", "control", &LowS, 255); //Saturation (0 - 255)
	//createTrackbar("HighS", "control", &HighS, 255);
	//createTrackbar("LowV", "control", &LowV, 255);//Value (0 - 255)
	//createTrackbar("HighV", "control", &HighV, 255);
	createTrackbar("Offset", "control", &yOff, 400);
	//createTrackbar("Thresh", "control", &thresh, 40);
	//createTrackbar("Min Len", "control", &minLen, 60);
	//createTrackbar("Max Gap", "control", &maxGap, 60);
	loadCamData();
}
void Alg::loadCamData() {
	FileStorage fs("out_camera_data.xml", FileStorage::READ);
	fs["camera_matrix"] >> camMat;
	fs["distortion_coefficients"] >> distCoef;
}

Mat Alg::process2(Mat img) {
	outFrame = img.clone();
	birdImg = transform(img.clone());

	maskImg = mask2(img.clone());
	filterImg = filter(maskImg.clone());
	
	houghImg = hough2(filterImg.clone());

	//imshow("outFrame", outFrame);

	line(outFrame, Point(int(outFrame.cols / 2), 0), Point(int(outFrame.cols / 2), outFrame.rows), white);

	return outFrame;
}

Mat Alg::transform(Mat src) {
	/*Point2f src_vertices[4] = { Pt(175, 44), Pt(275, 44), Pt(393, 139), Pt(56, 139) };
	Point2f dst_vertices[4] = { Pt(175, 44), Pt(275, 44), Pt(275, 139), Pt(175, 139) };*/
	Point2f src_vertices[6] = { l3, l2, l1, r1, r2, r3 };
	Point2f dst_vertices[6] = { Pt(l1.x, l3.y), Pt(l1.x, l2.y), l1, r1, Pt(r1.x, r2.y), Pt(r1.x, r3.y) };
	Mat pTrans = getPerspectiveTransform(src_vertices, dst_vertices);

	Point2f srcVert2[4] = { Pt(165, 44), Pt(285, 44), Pt(403, 139), Pt(46, 139) };
	Point2f dstVert2[4] = { Pt(165, 44), Pt(285, 44), Pt(285, 139), Pt(165, 139) };
	Mat pTrans2 = getPerspectiveTransform(srcVert2, dstVert2);


	Mat dst(src.size(), CV_8UC3);
	warpPerspective(src, dst, pTrans, dst.size(), INTER_LINEAR, BORDER_CONSTANT);


	Mat dst2;
	warpPerspective(src, dst2, pTrans2, dst.size(), INTER_LINEAR, BORDER_CONSTANT);

	//Mat dst3(calibImg, Rect(0, 189, src.cols, 189));
	//ROI(calibImg, srcVert2[3], srcVert2[1], )
	//warpPerspective(calibImg, dst3, pTrans2, dst3.size(), INTER_LINEAR, BORDER_CONSTANT);

	imshow("dst", dst);
	imshow("dst2", dst2);
//	imshow("dst3", dst3);
	return src;
}

Mat Alg::mask2(Mat img) {
	rectangle(img, Point(0, yOff), Point(img.cols, 0), black, -1);
	line(outFrame, Point(0, yOff), Point(outFrame.cols, yOff), white);
	return img;
}

Mat Alg::filter(Mat hsvImg) {
	cvtColor(hsvImg, hsvImg, COLOR_BGR2HSV);
	inRange(hsvImg, Scalar(LowH, LowS, LowV), Scalar(HighH, HighS, HighV), hsvImg);
	//morphological opening (removes small objects from the foreground)
	erode(hsvImg, hsvImg, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
	dilate(hsvImg, hsvImg, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
	//morphological closing (removes small holes from the foreground)
	dilate(hsvImg, hsvImg, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
	erode(hsvImg, hsvImg, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
	return hsvImg;
}

Mat Alg::hough2(Mat img) {
	vector<Vec4i> testVec;
	HoughLinesP(img, testVec, 1, CV_PI / 180, thresh, minLen, maxGap);
	vector<t7> lines, greenLines, redLines;
	t7 yellow, orange;
	for (Vec4i v : testVec)
		lines.emplace_back(v);
	for (t7 t : lines) {
		if (t.m > 0) {
			line(outFrame, t.p1(), t.p2(), green, 1, 8);
			greenLines.emplace_back(t);
		}
		else {
			line(outFrame, t.p1(), t.p2(), red, 1, 8);
			redLines.emplace_back(t);
		}
		//if (!greenLines.empty()) {
		//	orange = getAveraget7(greenLines);
		//	line(outFrame, orange.p1(), orange.p2(), orange, 1, 8);
		//}
		//if (!redLines.empty()) {
		//	yellow = getAveraget7(redLines);
		//	line(outFrame, yellow.p1(), yellow.p2(), yellow, 1, 8);
		//}
	}
	return img;
}

t7 Alg::getAveraget7(vector<t7> vec) {
	if (!vec.empty()) {
		int botX = 0, botY = 0, topX = 0, topY = 0;
		t7 aveLine = t7(0, 0, 0, 0);
		for (t7 t : vec) {
			botX += t[0];
			botY += t[1];
			topX += t[2];
			topY += t[3];
		}
		botX /= (int)vec.size();
		botY /= (int)vec.size();
		topX /= (int)vec.size();
		topY /= (int)vec.size();
		return t7(botX, botY, topX, topY);
	}
	else
		return t7(0, 0, 0, 0);
}
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
//	show(blur2, 1, "thresh");
	HoughLinesP(blur2, testVec, 1, CV_PI/180, 10, 0, 50);
	//cout << testVec.size() << "\n";
	for(Vec4i v : testVec)
		line(inSmall, Point(v[0], v[1]), Point(v[2], v[3]), white, 1, 8);
	

//	show(grayImg, 1, "grayImg");
//	show(blur2top, 1, "blur2top");
//	show(blur2bot, 1, "blur2bot");
//	show(blur2, 0, "blur2");


//	blur2 = superBlur(blur2);


//	blurImg = superBlur(grayImg.clone());				//2 blurImg
//	show(inSmall, 1, "inSmall");
//	show(blurImg, 1, "blurImg");
//	show(blur2, 1, "blur2");
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