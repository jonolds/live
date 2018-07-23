#ifndef VIDCONT_H
#define VIDCONT_H
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include "Alg.h"

class VidCont {
public:
	void drawDots(cv::Mat&);
	Pt l1 = Pt(172, 67), l2 = Pt(142, 90), l3 = Pt(0, 202), l4 = Pt(0, 0), r1 = Pt(260, 69), r2 = Pt(294, 93), r3 = Pt(448, 214), r4 = Pt(0, 0);

	cv::Mat camMat, distCoef;
	void loadCamData();

	int totalFrames, framePos = 0, sliderNumber = 0;
	double freq;
	cv::Mat inFrame, outFrame;
	cv::VideoCapture reader;
	cv::VideoWriter writer;
	Alg* mod;
	int fps = 10, frameCount = 0;
	std::string outVid, outWin = "out";
	bool isVidOpen = false, isWrInit = false;
	//VidCont member functions
	VidCont(const std::string& inVidPath = "", std::string outVidPath = "");
	~VidCont();
	void initWriter(), endAll(), run();
};
#endif
