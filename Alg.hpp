#ifndef _ALG_H_
#define  _ALG_H_
#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
#include <math.h>
#include <iomanip>
#include <numeric>
#include "helper.hpp"
using namespace std; using namespace cv; using namespace viz;
class Alg {
public:
	Vec4i gAveLine, rAveLine;
	vector<Vec4i> farSlopeVec;
	deque<double> angleSums;
	int y_offset, frameCountAlg = 0, houghThresh = 30, minAngle, maxAngle;
	double lowThresh = 50, highThresh = 150, minLength = 30, maxGap = 20, resizeFactor = .35, offsetFactor = .2;
	bool l2gradient = false, test = false;
	Point topLeftP, topMidLeftP, topMidRightP, topRightP, topMidP, leftSideP, rightSideP, botLeftP, botRightP;
	Point gTopPt, gBotPt, rTopPt, rBotPt;
	Alg() {}; ~Alg() { cvDestroyAllWindows(); }
	Mat initMats(Mat src, Mat&  out), applyCanny(Mat src_gray);
	void process(Mat& src, Mat& out), applyHoughP(Mat& im_edge, Mat& out, vector<Vec4i>& lines);
	void sortLines(Mat& img, vector<Vec4i>& lines, vector<Vec4i>& greens, vector<Vec4i>& reds);
	void drawLines(Mat& out, vector<Vec4i> greens, vector<Vec4i> reds);
	void drawOrAndYel(Mat& out), drawMarks(Mat& out);
};

void Alg::process(Mat& src, Mat& out) {
	/*setCanny*/lowThresh = 30, highThresh = 105, l2gradient = false, /*setHough*/ houghThresh = 15, minLength = 20, maxGap = 70;
	gAveLine = Vec4i::all(0); rAveLine = Vec4i::all(0);
	vector<Vec4i> lines, greens, reds, greenFrameLines, redFrameLines;
	
	Mat src_gray = initMats(src, out);
	y_offset = int(offsetFactor * out.rows);

	Mat edges = applyCanny(src_gray);
	applyHoughP(edges, out, lines);
	
	sortLines(out, lines, greens, reds);
	drawLines(out, greens, reds);
	drawMarks(out);
}
Mat Alg::applyCanny(Mat src_gray) {
	blur(src_gray, src_gray, Size(5, 5));
	GaussianBlur(src_gray, src_gray, Size(7, 7), 4);
	GaussianBlur(src_gray, src_gray, Size(5, 7), 4);
	Mat edges(src_gray.size(), CV_8U, Scalar::all(0));
	Canny(src_gray, edges, lowThresh, highThresh, 3, l2gradient);
	
	rectangle(edges, Point(0, y_offset), Point(src_gray.cols, 0), Scalar::all(0), -1);
	Point triangle[3] = {Point(int(.1*edges.cols), edges.rows), Point(int(.9*edges.cols), edges.rows), Point(int(.5*edges.cols), y_offset)};
	fillConvexPoly(edges, triangle, 3, Scalar::all(255));
	
	imshow("canny", edges);
	
	fillConvexPoly(edges, triangle, 3, Scalar::all(0));
	return edges;
}
void Alg::applyHoughP(Mat& edges, Mat& out, vector<Vec4i>& lines) {
	HoughLinesP(edges, lines, 1, 1 * CV_PI / 180, houghThresh, minLength, maxGap); //1st best{30, 20, 20}>{30,10,20}>{40, 20, 10} 
	line(out, Point(0, y_offset), Point(out.size().width, y_offset), Color::pink(), 1, 8);
}
void Alg::sortLines(Mat& img, vector<Vec4i>& lines, vector<Vec4i>& greens, vector<Vec4i>& reds) {
	for (size_t i = 0; i < lines.size(); i++) {
		int x1, y1, x2, y2;
		double angle = getAngle(lines[i], x1, y1, x2, y2);
		if ((abs(angle) > 20)&&(abs(angle) < 62)) {
			if ((angle > 0)  &&  (x1 > topMidRightP.x)  &&  (x2 > topMidRightP.x))
					reds.emplace_back(lines[i]);
			else if((angle < 0)  &&  (x1 < topMidLeftP.x)  &&  (x2 < topMidLeftP.x))
					greens.emplace_back(lines[i]);
		}
	}
}
void Alg::drawLines(Mat& out, vector<Vec4i> greens, vector<Vec4i> reds) {
	if (greens.size() > 0) {
		gAveLine = std::accumulate(greens.begin(), greens.end(), Vec4i(0, 0, 0, 0))/ int(greens.size());
		line(out, Point(gAveLine[0], gAveLine[1]), Point(gAveLine[2], gAveLine[3]), Color::green(), 1, 8);
		boxWrite(out, decStr(getAngle(gAveLine)), Point(10, 42));

		//green top/bottom pts and line
		double xTop = (y_offset - gAveLine[1])/getSlope(gAveLine) + gAveLine[0];
		double yIntercept = gAveLine[1] - getSlope(gAveLine)*gAveLine[0];
		gTopPt = Point(xTop, y_offset);
		if (yIntercept < out.rows)
			gBotPt = Point(0, int(yIntercept));
		else {
			double xBottom = (out.rows - gAveLine[1]) / getSlope(gAveLine) + gAveLine[0];
			gBotPt = Point(int(xBottom), out.rows);
		}
		line(out, gTopPt, gBotPt, Color::orange(), 1, 8);
		circle(out, gBotPt, 4, Color::white(), -1, 8);
		circle(out, gTopPt, 4, Color::white(), -1, 8);
	}
	if (reds.size() > 0) {
		rAveLine = std::accumulate(reds.begin(), reds.end(), Vec4i(0, 0, 0, 0)) / int(reds.size());
		line(out, Point(rAveLine[0], rAveLine[1]), Point(rAveLine[2], rAveLine[3]), Color::red(), 1, 8);
		boxWrite(out, decStr(getAngle(rAveLine)), Point(300, 42));

		//red top/bottom pts and line
		double xTop = (y_offset - rAveLine[1]) / getSlope(rAveLine) + rAveLine[0];
		double yIntercept = getSlope(rAveLine)*(out.cols - rAveLine[0])+rAveLine[1];
		rTopPt = Point(int(xTop), y_offset);
		if (yIntercept < out.rows) 
			rBotPt = Point(out.cols, int(yIntercept));
		else {
			double xBottom = (out.rows - rAveLine[1]) / getSlope(rAveLine) + rAveLine[0];
			rBotPt = Point(int(xBottom), out.rows);
		}
		line(out, rTopPt, rBotPt, Color::orange(), 1, 8);
		circle(out, rBotPt, 4, Color::white(), -1, 8);
		circle(out, rTopPt, 4, Color::white(), -1, 8);

		if(greens.size() > 0) //if both are > 0
			angleSums.emplace_front(getAngle(rAveLine) + getAngle(gAveLine));
	}
	if (angleSums.size() == 9) {
		double angleAve = std::accumulate(angleSums.begin(), angleSums.end(), 0.0)/9;
		boxWrite(out, decStr(angleAve), Point(170, 42));
		angleSums.pop_back();
		if (abs(angleAve) > 5) 
			drawWarnArrows(out, angleAve);
	}
}
void Alg::drawMarks(Mat& out) {
	y_offset = int(offsetFactor * out.rows);
	Point yelPts[9] = { topMidP = Point(int(.5*out.cols), y_offset),
		topLeftP = Point(int(.23*out.cols), y_offset),		topMidLeftP = Point(int(.45*out.cols), y_offset),
		topMidRightP = Point(int(.55*out.cols), y_offset),	topRightP = Point(int(.77*out.cols), y_offset),
		botLeftP = Point(int(.25*out.cols), out.rows),		botRightP = Point(out.cols - int(.25*out.cols), out.rows),
		leftSideP = Point(0, int(.34*out.rows)),			rightSideP = Point(out.cols, int(.34*out.rows))
	};
	for (int i = 0; i < 9; i++)
		circle(out, yelPts[i], 3, Color::yellow(), -1, 8);
}
Mat Alg::initMats(Mat src, Mat& out) {
	resize(src, src, Size(int(resizeFactor*src.size().width), int(resizeFactor*src.size().height)));
	out = src.clone();
	Mat src_gray;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	return src_gray;
	Mat tmp = Mat(src.size(), COLOR_GRAY2BGR);
}
void Alg::drawOrAndYel(Mat& out) {
	line(out, Point(farSlopeVec[0][0], farSlopeVec[0][1]), Point(farSlopeVec[0][2], farSlopeVec[0][3]), Color::yellow(), 2, LINE_8, 0);
	int num = 6;
	if (num < int(farSlopeVec.size())) {
		farSlopeVec.pop_back();
		Vec4i sumLastSix(farSlopeVec[0][0], farSlopeVec[0][1], farSlopeVec[0][2], farSlopeVec[0][3]);
		for (int i = 1; i < 6; i++) {
			sumLastSix[0] += farSlopeVec[i][0], sumLastSix[1] += farSlopeVec[i][1];
			sumLastSix[2] += farSlopeVec[i][2], sumLastSix[3] += farSlopeVec[i][3];
		}
		Vec4i aveLastSix(int(sumLastSix[0] / 6), int(sumLastSix[1] / 6), int(sumLastSix[2] / 6), int(sumLastSix[3] / 6));
		line(out, Point(aveLastSix[0], aveLastSix[1]), Point(aveLastSix[2], aveLastSix[3]), Color::orange(), 2, LINE_8, 0);
	}
	frameCountAlg++;
}
#endif