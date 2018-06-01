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

class Alg {
public:
	cv::Mat inSmall, grayImg, blurImg, canImg, maskImg;
	cv::Mat mskHghBl, mskLwBl, mskFullBl, cnMskImg;
	cv::Mat mskHghYel, mskLwYel, mskFullYel;
	cv::Mat houghImg, outFrm, grid, side;
	
	t7 grAve, grAveTop, grAveBot;
	t7 rAve, rAveTop, rAveBot;
	
	t7vec allLns, allLnsTop, allLnsBot;
	t7vec grLns, grLnsTop, grLnsBot;
	t7vec rLns, rLnsTop, rLnsBot;
	t7vec badLns, badLnsTop, badLnsBot;
	
	
	deqD angleSumsDeq;
	int yOff, frCntAlg = 0, hThresh = 15, minAngle, maxAngle, rows, cols;
	double lowThr = 20, highThr = 50, minLen = 20, maxGap = 70, offsetFactor = .18;
	cv::Point topLPt, topMidLPt, topMidRPt, topRightPt, topMidPt, sideLPt, sideRPt, botLPt, botRPt;

	Alg();
	~Alg() { cvDestroyAllWindows(); }
	cv::Mat process(cv::Mat);
	void init(cv::Mat inFrame);
	
	void mskAll(cv::Mat mskIn);
	cv::Mat getMskHgh(cv::Mat cnImg, cv::Scalar color);
	cv::Mat getMskLw(cv::Mat cnImg, cv::Scalar color);
	cv::Mat getMskInit(cv::Mat cnImg, cv::Scalar color);
	cv::Mat getHough(cv::Mat input, cv::Mat& output);
	void sortHoughLines(Alg& alg);
	
	
	cv::Mat getOutFrame(cv::Mat img);
	void drawMarks(cv::Mat& outMat);
	cv::Mat cleanAndReturn();
	void showImages();
};
#endif
