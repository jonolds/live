#ifndef ALG_H
#define  ALG_H
#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
#include <math.h>
#include <iomanip>
#include <numeric>
class Alg;
const enum COLOR : int { RED, GREEN };
using namespace std; using namespace cv; using namespace viz;

struct LaneLine {
	LaneLine() {};
	LaneLine(int col, Alg* alg) {
		color = bool(col);
		aveLine = Vec4i::all(0);
		mod = alg;
	};
	~LaneLine() {}
	Alg *mod;
	Vec4i aveLine;
	Point topPt, botPt;
	vector<Vec4i> lines;
	deque<Point> topPtDeq, botPtDeq;
	bool color;
};

class Alg {
public:
	Alg() {
		g = LaneLine(GREEN, this);
		r = LaneLine(RED, this);
	};
	~Alg() { cvDestroyAllWindows(); }
	LaneLine g, r;
	deque<double> angleSumsDeq;
	int y_offset, frCntAlg = 0, hThresh, minAngle, maxAngle;
	double lowThr, highThr, minLen, maxGap, resizeFactor = .35, offsetFactor = .2;
	Point topLPt, topMidLPt, topMidRPt, topRightPt, topMidPt, sideLPt, sideRPt, botLPt, botRPt;
	Point nextTriLeft, nextTriRight;
	Mat init(Mat src, Mat&  out); 
	void process(Mat& src, Mat& out); 
	void applyCannyHough(Mat src_gray, Mat& out, vector<Vec4i>& lines);
	void sortLines(Mat& out, vector<Vec4i>& lines);
	void drawLaneLines(Mat& out, LaneLine& lane);
	void drawMarks(Mat& out);
};
#endif