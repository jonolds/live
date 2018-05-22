#ifndef ALG_H
#define ALG_H
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>


class Alg;
enum COLOR : int { RED, GREEN };
using namespace std; 
using namespace cv; 

struct LaneLine {
	LaneLine() { mod = NULL; color = NULL; };
	LaneLine(int col, Alg* alg) {
		color = bool(col);
		aveFrLine = Vec4i::all(0);
		mod = alg;
	};
	~LaneLine() = default;
	Alg *mod;
	Vec4i aveFrLine;
	Point topPt, botPt;
	vector<Vec4i> lines;
	deque<Point> topPtDeq, botPtDeq;
	bool color;
};

class Alg {
public:
	Mat inSmall, grayImg, blurImg, cannyImg, maskDispImg, cannyMaskedImg, houghImg, outFrame;
	LaneLine *gTop, *rTop;//, *gBot, *rBot;
	deque<double> angleSumsDeq;
	int y_offset, frCntAlg = 0, hThresh, minAngle, maxAngle;
	int rows, cols;
	double lowThr, highThr, minLen, maxGap, offsetFactor = .2;
	Point topLPt, topMidLPt, topMidRPt, topRightPt, topMidPt, sideLPt, sideRPt, botLPt, botRPt;
	Point nextTriLeft, nextTriRight;

	Alg();//, gBot = new LaneLine(GREEN, this), rBot = new LaneLine(RED, this); }
	~Alg() { cvDestroyAllWindows(); delete gTop; delete rTop;}
	Mat process(Mat inFrame);
	Mat init(Mat inFrame);
	Mat superBlur();
	Mat canny();
	Mat mask(Mat img, Scalar color);
	Mat hough(vector<Vec4i> & lines);
	void sortLines(vector<Vec4i>& lines);
	void drawLaneLines(Mat& outMat, LaneLine *lane);
	void drawMarks(Mat& outMat);
	
	//helper
	Mat drawLaneLines(LaneLine *lane, LaneLine *lane2);
	void showImages();
};
#endif