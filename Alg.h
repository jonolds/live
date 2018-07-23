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
	cv::Mat inSmall, grayImg, blurImg, canImg;
	cv::Mat maskTop, maskBot, maskAll, maskGeneric;
	cv::Mat mskHghYel, mskLwYel, mskFullYel;
	
	
	cv::Mat outFrm, grid, side;


	std::vector<Sector> sectors;
	std::vector<Pt> rhombus;
	std::vector<Pt> keyPts;
	
	deqD angleSumsDeq;
	int frCntAlg = 0, minAngle, maxAngle, rows, cols;
	double lowThr = 20, highThr = 50, offsetFactor = .2;
	cv::Point topLPt, topMidLPt, topMidRPt, topRightPt, topMidPt, sideLPt, sideRPt, botLPt, botRPt;

	Alg();
	~Alg() { cv::destroyAllWindows(); }
	cv::Mat process(cv::Mat);


	t7 getAveraget7(std::vector<t7>);


	//****************************
	Pt l1 = Pt(172, 67), l2 = Pt(142, 90), l3 = Pt(0, 202), l4 = Pt(0, 0), r1 = Pt(260, 69), r2 = Pt(294, 93), r3 = Pt(448, 214), r4 = Pt(0, 0);

	cv::Mat camMat, distCoef;
	void loadCamData();

	cv::Mat inFrame, maskImg, filterImg, birdImg, houghImg, outFrame;
	cv::Mat process2(cv::Mat);

	cv::Mat transform(cv::Mat);

	int yOff = 44;
	cv::Mat mask2(cv::Mat);

	int LowH = 75, HighH = 140, LowS = 105, HighS = 248, LowV = 0, HighV = 255;
	cv::Mat filter(cv::Mat);

	
	
	int thresh = 13, minLen = 7, maxGap = 3;
	cv::Mat hough2(cv::Mat);



	//****************************
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