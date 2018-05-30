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
	vconcat(row1, row2, row1);
	vconcat(row1, row3, row1);
	return row1;
}

Mat catRows(Mat col1, Mat col2, Mat col3) {
	hconcat(col1, col2, col1);
	hconcat(col1, col3, col1);
	return col1;
}

Mat cvtColGr(Mat mat) {
	cvtColor(mat, mat, mat.type() == 16 ? COLOR_BGR2GRAY : COLOR_GRAY2BGR);
	return mat;
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
