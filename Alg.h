#ifndef ALG_H
#define ALG_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "t7.h"
#include "t7vec.h"
#include "basics.h"

typedef std::vector<cv::Vec4i> vVec4i;
typedef std::deque<double> deqD;
typedef std::vector<t7> t7vec2;

class Alg {
public:
	cv::Mat inSmall, grayImg, blurImg, canImg, maskImg;
	cv::Mat mskHghBl, mskLwBl, mskFullBl, cnMskImg;
	cv::Mat mskHghYel, mskLwYel, mskFullYel;
	cv::Mat houghImg, outFrm, grid, side;

	t7vec allLns, allLnsTop, allLnsBot;
	t7vec grLns, grLnsTop, grLnsBot;
	t7vec rLns, rLnsTop, rLnsBot;
	t7vec badLns, badLnsTop, badLnsBot;
	
	deqD angleSumsDeq;
	int yOff, frCntAlg = 0, hThresh = 15, minAngle, maxAngle, rows, cols;
	double lowThr = 20, highThr = 50, minLen = 20, maxGap = 170, offsetFactor = .05;
	cv::Point topLPt, topMidLPt, topMidRPt, topRightPt, topMidPt, sideLPt, sideRPt, botLPt, botRPt;

	Alg();
	~Alg() { cvDestroyAllWindows(); }
	cv::Mat process(cv::Mat);
	void init(cv::Mat inFrame);
	
	void mskAll(cv::Mat mskIn);
	cv::Mat getMsk(cv::Mat cnImg, cv::Scalar color, Pt p1 = Pt(0, 0), Pt p2 = Pt(0, 0));
	cv::Mat getHough();
	void sortHoughLines(t7vec& allVec, t7vec& rVec, t7vec& gVec, t7vec& bVec);
	
	cv::Mat getOutFrame(cv::Mat img);
	void drawMarks(cv::Mat& outMat);
	cv::Mat cleanAndReturn();
	void showImages();
};
#endif
