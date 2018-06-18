#ifndef ALG_H
#define ALG_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include "t7.h"
#include "Sector.h"
#include "basics.h"

class Sector;
typedef std::deque<double> deqD;

class Alg {
public:
	cv::Mat inSmall, grayImg, blurImg, canImg, maskImg;
	cv::Mat maskTop, maskBot, maskAll, maskGeneric;
	cv::Mat mskHghYel, mskLwYel, mskFullYel;
	cv::Mat houghImg, outFrm, grid, side;

	std::vector<Sector> sectors;
	std::vector<Pt> rhombus;
	std::vector<Pt> keyPts;
	
	deqD angleSumsDeq;
	int yOff, frCntAlg = 0, minAngle, maxAngle, rows, cols;
	double lowThr = 20, highThr = 50, offsetFactor = .2;
	cv::Point topLPt, topMidLPt, topMidRPt, topRightPt, topMidPt, sideLPt, sideRPt, botLPt, botRPt;

	Alg(){};
	~Alg() { cvDestroyAllWindows(); }
	cv::Mat process(cv::Mat);
	void init(cv::Mat inFrame);
	
	void mskAll(cv::Mat mskIn);
	cv::Mat createMask(cv::Mat cnImg, cv::Scalar color, Pt p1 = Pt(0, 0), Pt p2 = Pt(0, 0));
	cv::Mat createGenericMask(cv::Mat cnImg);
	void createSectors();

	void drawMarks(cv::Mat& outMat);

	void paintAll(std::vector<Sector> secs);

	cv::Mat cleanAndReturn();
	void showImages();
};
#endif