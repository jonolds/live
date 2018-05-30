#ifndef T7VEC_HPP
#define T7VEC_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <numeric>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "t7.hpp"
using namespace std;
using namespace cv;

class t7vec : public vector<t7> {
public:
	t7vec() {}
	t7vec(Scalar color) {
		col = color;
	};
	Scalar col;
	t7 avLn;
	void drwVecLns(Mat& img);
	t7 drwAv(Mat& img, int offset = 0);
	t7 getAvLn();
};
inline void t7vec::drwVecLns(Mat& img) {
	for (int i = 0; i < size(); i++)
		line(img, Point(at(i)[0], at(i)[1]), Point(at(i)[2], at(i)[3]), col);
}
inline t7 t7vec::drwAv(Mat& img, int offset) {
	avLn = getAvLn();
	line(img, avLn.p1() + Point(0, offset), avLn.p2() + Point(0, offset), col, 2);
	return avLn;
}
inline t7 t7vec::getAvLn() {
	return t7(std::accumulate(begin(), end(), Vec4i(0, 0, 0, 0)) / int(size()));
}
#endif