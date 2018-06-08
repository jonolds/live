#ifndef SECTOR_H
#define SECTOR_H
#include <opencv2/core.hpp>
#include <vector>
#include "t7vec.h"

class sector : public t7vec {
public:
	sector() {}
	sector(cv::Scalar color) {
		col = color;
	};
	//cv::Scalar col;
	//t7 avLn;
	//void drawVecLns(cv::Mat& img);
	//t7 drawAvLn(cv::Mat& img, int offset = 0);
	//t7 getAvLn();
};

#endif
