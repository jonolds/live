#include "basics.h"
#include "colors.h"

using namespace std;
using namespace cv;

void boxWrite(Mat& mat, string str, Point pt, Scalar boxCol) {
	int bs = 0;
	Size sz = getTextSize(str, 4, .5, 1, &bs);
	rectangle(mat, Point(pt.x, pt.y + 2), Point(pt.x + sz.width, pt.y - sz.height - 1), boxCol, -1, 8);
	putText(mat, str, pt, /*FONT*/ 4, .5, white, 1, 8);
}

Mat catCols(Mat row1, Mat row2, Mat row3) {
	row1 = cvtCol(row1); row2 = cvtCol(row2); row3 = cvtCol(row3);
	vconcat(row1, row2, row1);
	vconcat(row1, row3, row1);
	return row1;
}

Mat catRows(Mat col1, Mat col2, Mat col3) {
	col1 = cvtCol(col1); col2 = cvtCol(col2); col3 = cvtCol(col3);
	hconcat(col1, col2, col1);
	hconcat(col1, col3, col1);
	return col1;
}

Mat cvtCol(Mat img) {
	if(img.type() == 0)
		cvtColor(img, img, COLOR_GRAY2BGR);
	return img;
}

Mat cvtGr(Mat img) {
	if (img.type() != 0)
		cvtColor(img, img, COLOR_BGR2GRAY);
	return img;
}

void show(Mat img, int time, string winName) {
	imshow(winName, img);
	waitKey(time);
}

string decStr(double num) {
	ostringstream obj;
	obj << (abs(num) < 1 ? setprecision(2) : abs(num) < 10 ? setprecision(3) : setprecision(4)) << num;
	return obj.str();
}

void drawArrow(Mat& out, bool neg) {
	arrowedLine(out, Point(neg ? 60 : out.cols - 60, 15), Point(neg ? 20 : out.cols - 20, 15), black, 7, FILLED, 0, 0.25);
	arrowedLine(out, Point(neg ? 60 : out.cols - 60, 15), Point(neg ? 20 : out.cols - 20, 15), green, 2, FILLED, 0, 0.25);
}

Mat getSolidImg(Mat img, Scalar color) {
	return Mat(img.size(), img.type(), color);
}
Mat rectangleRet(Mat img, Point p1, Point p2, Scalar color, int thickness) {
	Mat img2 = img.clone();
	rectangle(img2, p1, p2, color, thickness, 8, 0);
	return img2;
}
void drawLineByIter(Mat& img, Point a, Point b, Scalar color) {
	LineIterator lit(img, a, b);
	for (int i = 0; i < lit.count; i++, ++lit) {
		Vec3b& px = img.at<Vec3b>(lit.pos());
		px = Vec3b(uchar(color[0]), uchar(color[1]), uchar(color[2]));
	}
}

Mat reSz(Mat img, double factor) {
	resize(img, img, Size(int(factor * img.cols), int(factor * img.rows)));
	return img;
}