#ifndef ALG_H
#define ALG_H
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/core.hpp>
#include "t7i.hpp"
#include <vector>

#include <cmath>
#include <iomanip>
#include <numeric>

class Alg;
enum COLOR : int { RED, GREEN };
using namespace std; 
using namespace cv;

typedef vector<Vec4i> vVec4i;
typedef deque<double> deqD;
typedef vector<t7i> vecT7;

class Alg {
public:
	Mat inSmall, grayImg, blurImg, canImg, maskImg, canMaskImg, houghImg, outFrm;
	t7i gAve, rAve;
	vecT7 allLns, gLns, rLns, badLns;
	deqD angleSumsDeq;
	int yOff, frCntAlg = 0, hThresh = 15, minAngle, maxAngle, rows, cols;
	double lowThr = 20, highThr = 50, minLen = 20, maxGap = 70, offsetFactor = .18;
	Point topLPt, topMidLPt, topMidRPt, topRightPt, topMidPt, sideLPt, sideRPt, botLPt, botRPt;

	Alg();
	~Alg() { cvDestroyAllWindows();}
	Mat process(Mat);
	void init(Mat inFrame);
	Mat getBlur(Mat grImg);
	Mat getCanny(Mat blrImg);
	Mat getMask(Mat cnImg, Scalar color);
	Mat getHough();
	Mat getOutFrame();
	void drawLaneLine(Mat& outMat, t7i t);
	void drawMarks(Mat& outMat);
	Mat cleanAndReturn();
	void showImages();
};
#endif