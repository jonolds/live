#ifndef VIDCONT_H
#define VIDCONT_H
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include "Alg.h"

class VidCont {
public:
	cv::VideoCapture reader;
	cv::VideoWriter writer;
	Alg* mod;
	int fps = 10, frameCount = 0;
	std::string outVid, outWin = "out";
	bool isVidOpen = false, isWrInit = false;
	//VidCont member functions
	VidCont(std::string inVidPath = "", std::string outVidPath = "");
	~VidCont();
	void initWriter(), endAll(), run(), setSize(cv::VideoCapture reader, cv::Size reqSize);
	cv::Size getSize(cv::VideoCapture vc);
};
#endif
