#ifndef T7VEC_H
#define T7VEC_H
#include <opencv2/core.hpp>
#include <vector>
#include "t7.h"
#include "Alg.h"
#include "colors.h"


class Sector {
public:
	cv::Scalar color, leftColor, rightColor;
	cv::Mat mask, genericMaskIn;
	t7 avLn;
	std::vector<t7> left, right, all, bad;
	
	Sector() {
		init();
	}
	Sector(cv::Scalar col) {
		color = col;
		init();
	}
	Sector(Pt pt, int num, cv::Mat mskIn);
	void getHough();
	void sortHoughLines();
	void init();
	int size();	
	void processSector();
	void paintLines(cv::Mat& imgToPaint);
};
#endif