#ifndef ALG_H
#define ALG_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "t7.h"
#include "t7vec.h"


typedef vector<cv::Vec4i> vVec4i;
typedef deque<double> deqD;
typedef vector<t7> t7vec2;

class Alg
{
public:
	cv::Mat inSmall, grayImg, blurImg, canImg, maskImg, maskHigh, maskLow, canMaskImg, canMaskHigh, canMaskLow, houghImg, outFrm, grid, side;
	t7 gAve, rAve;
	t7vec allLns, gLns, rLns, badLns;
	deqD angleSumsDeq;
	int yOff, frCntAlg = 0, hThresh = 15, minAngle, maxAngle, rows, cols;
	double lowThr = 20, highThr = 50, minLen = 20, maxGap = 70, offsetFactor = .18;
	cv::Point topLPt, topMidLPt, topMidRPt, topRightPt, topMidPt, sideLPt, sideRPt, botLPt, botRPt;

	Alg();
	~Alg() { cvDestroyAllWindows(); }
	cv::Mat process(cv::Mat);
	void init(cv::Mat inFrame);
	cv::Mat getBlur(cv::Mat grImg);
	cv::Mat getCanny(cv::Mat blrImg);
	cv::Mat getMaskHigh(cv::Mat cnImg, cv::Scalar color, int yLow);
	cv::Mat getMaskLow(cv::Mat cnImg, cv::Scalar color, int yLow);
	cv::Mat getHough();
	void sortHoughLines(Alg& alg);
	cv::Mat getOutFrame(cv::Mat img);
	void drawMarks(cv::Mat& outMat);
	cv::Mat cleanAndReturn();
	void showImages();
};
#endif
