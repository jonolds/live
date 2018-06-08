#ifndef T7VEC_H
#define T7VEC_H
#include <opencv2/core.hpp>
#include <vector>
#include "t7.h"

class t7vec : public std::vector<t7> {
public:
	t7vec() {}

	t7vec(cv::Scalar color) {
		col = color;
	};
	cv::Scalar col;
	t7 avLn;
	void drawVecLns(cv::Mat& img);
	t7 drawAvLn(cv::Mat& img, int offset = 0);
	t7 getAvLn();
};

#endif
