#include <numeric>
#include "Sector.h"
#include "draw.h"

using namespace std;
using namespace cv;

void Sector::init() {
	left = std::vector<t7>();
	right = std::vector<t7>();
	all = std::vector<t7>();
	bad = std::vector<t7>();
}
int Sector::size() {
	return (int)all.size();
}
Sector::Sector(Pt pt, int num, cv::Mat mskIn) {
	genericMaskIn = mskIn;
	if (num%2 == 0) {
		leftColor = orange;
		rightColor = magenta;
	} else {
		leftColor = magenta;
		rightColor = orange;
	}
	mask = Mat(mskIn.size(), mskIn.type(), Scalar::all(0));
	mskIn.rowRange(pt.x, pt.y).copyTo(mask.rowRange(pt.x, pt.y));
	//show(mask);
	getHough();
//	cout << all.size() << "\n";

}

void Sector::getHough() {
	int hThresh = 15;
	double minLen = 20, maxGap = 170;

	vector<Vec4i> tmp;
	HoughLinesP(mask, tmp, .3, CV_PI/720, hThresh, minLen, maxGap);
	for (const Vec4i& t : tmp)
		all.emplace_back(t7(t));
}
void Sector::sortHoughLines() {
	for (t7& t : all) {
		if ((t.ang > 15) && (t.ang < 85) && (t[0] > mask.cols*55/100) && (t[2] > mask.cols*55/100))
			right.emplace_back(t);
		else if ((t.ang < -15) && (t.ang > -85) && (t[0] < mask.cols*45/100) && (t[2] < mask.cols*45/100))
			left.emplace_back(t);
		else
			bad.emplace_back(t);
	}
}

void Sector::paintLines(Mat& imgToPaint) {
	for (t7 t: left)
		line(imgToPaint, t.p1(), t.p2(), leftColor);
	for (t7 t: right)
		line(imgToPaint, t.p1(), t.p2(), rightColor);
}

void Sector::processSector() {
	
}
