#include "helper.h"
#include "basics.h"
#include "colors.h"
#include "draw.h"

using namespace std;
using namespace cv;



Mat getVanImg(Mat img, t7 gAve, t7 rAve, Point vanish) {
	Mat tmpGr = getSolidImg(img, black);
	vconcat(tmpGr, img, tmpGr);
	line(tmpGr, Point(gAve[2], gAve[3] + img.rows), vanish + Point(0, img.rows), white, 2);
	line(tmpGr, Point(rAve[0], rAve[1] + img.rows), Point(vanish.x, vanish.y + img.rows), white, 2);
	circle(tmpGr, Point(vanish.x, vanish.y + img.rows), 3, blue, -1);
	string s[] = {"vanish", " "};
	vector<Mat> m = {tmpGr, getSolidImg(img, gray)};
	label(m, s);
	vconcat(m[0], m[1], m[0]);
	vconcat(Mat(Size(m[0].cols, 28), m[0].type(), gray), m[0], m[0]);
	return m[0];
}





//Mat maskTop(Mat mask, Scalar color, int y_offset, int rows, int cols) {
//	rectangle(mask, Point(0, y_offset), Point(cols, 0), color, -1);
//	Point rhombus[4] = {Point(int(.1 * cols), rows), Point(int(.4 * cols), y_offset), Point(int(.6 * cols), y_offset), Point(int(.9 * cols), rows)};
//	fillConvexPoly(mask, rhombus, 4, color);
//	return mask;
//}

Mat drawHoughLines(Mat img, t7vec gLns, t7vec rLns, t7vec badLns) {
	gLns.drwVecLns(img);
	rLns.drwVecLns(img);
	badLns.drwVecLns(img);
	return img;
}

Mat superBlur(Mat img) {
	blur(img, img, Size(5, 5));
	GaussianBlur(img, img, Size(7, 7), 4);
	GaussianBlur(img, img, Size(5, 7), 4);
	return img;
}

Mat canny(Mat blrImg, double lowThrsh, double highThrsh) {
	Canny(blrImg, blrImg, lowThrsh, highThrsh);
	return blrImg;
}
