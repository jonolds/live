#include "helper.h"
#include "basics.h"
#include "colors.h"
#include "draw.h"

using namespace std;
using namespace cv;

Mat canny(Mat blrImg, double lowThrsh, double highThrsh) {
	Canny(blrImg, blrImg, lowThrsh, highThrsh);
	return blrImg;
}

Mat drawHoughLines(Mat img, Sector gLns, Sector rLns, Sector badLns) {
//	gLns.drawVecLns(img);
//	rLns.drawVecLns(img);
//	badLns.drawVecLns(img);
	return img;
}

Mat getVanImg(Mat img, t7 gAve, t7 rAve, Point vanish) {
	Mat tmpGr = getSolidImg(img, black);
	vconcat(tmpGr, img, tmpGr);
	line(tmpGr, Point(gAve[2], gAve[3] + img.rows), vanish + Point(0, img.rows), white, 2);
	line(tmpGr, Point(rAve[0], rAve[1] + img.rows), Point(vanish.x, vanish.y + img.rows), white, 2);
	circle(tmpGr, Point(vanish.x, vanish.y + img.rows), 3, blue, -1);
	string s[] = { "vanish", " " };
	vector<Mat> m = { tmpGr, getSolidImg(img, gray) };
	label(m, s);
	vconcat(m[0], m[1], m[0]);
	vconcat(Mat(Size(m[0].cols, 28), m[0].type(), gray), m[0], m[0]);
	return m[0];
}

Sector HoughLinesP_t7vec(Mat mat, double rho, double theta, int threshold, double minLen, double maxGap) {
	vector<Vec4i> tmp;
	Sector tmp_t7vec;
	HoughLinesP(mat, tmp, rho, theta, threshold, minLen, maxGap);
	//for (const Vec4i& t : tmp)
	//	tmp_t7vec.emplace_back(t7(t));
	return tmp_t7vec;
}

Mat superBlur(Mat img) {

	blur(img, img, Size(5, 5));
	blur(img, img, Size(5, 5));
	GaussianBlur(img, img, Size(7, 7), 4);
	GaussianBlur(img, img, Size(5, 7), 4);
	return img;
}