#include <numeric>
#include "t7vec.h"

using namespace std;
using namespace cv;

void t7vec::drwVecLns(Mat& img) {
	for (int i = 0; i < size(); i++) {
		line(img, Point(at(i)[0], at(i)[1]), Point(at(i)[2], at(i)[3]), col);
	}
}

t7 t7vec::drwAv(Mat& img, int offset) {
	avLn = getAvLn();
	line(img, avLn.p1() + Point(0, offset), avLn.p2() + Point(0, offset), col, 2);
	return avLn;
}

t7 t7vec::getAvLn() {
	return t7(std::accumulate(begin(), end(), Vec4i(0, 0, 0, 0)) / int(size()));
}
