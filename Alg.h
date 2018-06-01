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
	cv::Mat inSmall, grayImg, blurImg, canImg, maskImg;
	cv::Mat mskFull, mskHghBl, mskLwBl, cnMskImg;
	cv::Mat houghImg, outFrm, grid, side;
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
	
	
	void mskSplit(cv::Mat mskIn);
	cv::Mat getMskHgh(cv::Mat cnImg, cv::Scalar color);
	cv::Mat getMskLw(cv::Mat cnImg, cv::Scalar color);
	cv::Mat getMskInit(cv::Mat cnImg, cv::Scalar color);
	cv::Mat getHough();
	void sortHoughLines(Alg& alg);
	
	
	cv::Mat getOutFrame(cv::Mat img);
	void drawMarks(cv::Mat& outMat);
	cv::Mat cleanAndReturn();
	void showImages();
};
#endif
