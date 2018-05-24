#ifndef ALG_H
#define ALG_H
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>

class Alg;
enum COLOR : int { RED, GREEN };
using namespace std; 
using namespace cv; 

struct LaneLine {
	Alg *mod;
	Vec4i aveFrLine;
	Point topPt, botPt;
	vector<Vec4i> lines;
	bool color;
	void set(bool col, Alg* m) { 
		color = bool(col);
		mod = m;
	}
	void clear() {
		lines.clear(); aveFrLine = NULL; topPt = Point(-1, -1); botPt = Point(-1, -1);
	}
};

class Alg {
public:
	Mat inSmall, grayImg, blurImg, cannyImg, maskDispImg, cannyMaskedImg, houghImg, outFrame;
	LaneLine gTop, rTop;//, *gBot, *rBot;
	deque<double> angleSumsDeq;
	int y_offset, frCntAlg = 0, hThresh, minAngle, maxAngle;
	int rows, cols;
	double lowThr, highThr, minLen, maxGap, offsetFactor = .18;
	Point topLPt, topMidLPt, topMidRPt, topRightPt, topMidPt, sideLPt, sideRPt, botLPt, botRPt;
	Point nextTriLeft, nextTriRight;

	~Alg() { cvDestroyAllWindows();}
	Mat process(Mat);
	void init(Mat inFrame);
	void cannyHough();
	Mat mask(Mat img, Scalar color);
	void drawLaneLines(Mat& outMat, LaneLine lane);
	void drawMarks(Mat& outMat);
	
	//helper
	Mat drawLaneLines(LaneLine lane, LaneLine lane2);
	void showImages();
};
#endif