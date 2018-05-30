#include "draw.h"
#include "basics.h"

using namespace std;
using namespace cv;

void dot(Mat& mat, Point p, Scalar color, int radius) {
	circle(mat, p, radius, color, -1, 8);
}

void dotLns(Mat& mat, t7vec tVec, Scalar color, int y_off, int thickness) {
	for (t7 t : tVec) {
		line(mat, Point(t[0], t[1] + y_off), Point(t[2], t[3] + y_off), color, thickness);
		dot(mat, t.p1(), color, thickness + 2);
		dot(mat, t.p2(), color, thickness + 2);
	}
}

void label(vector<Mat>& vec, string* labels) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i].type() == 0) {
			cvtColor(vec[i], vec[i], COLOR_GRAY2BGR);
		}
		//add gray space for label and then add label
		vconcat(vec[i], Mat(Size(vec[i].cols, 28), vec[i].type(), gray), vec[i]);
		boxWrite(vec[i], labels[i], Point(5, vec[i].rows - 8), gray);
	}
}
void ln(Mat& mat, t7 t, Scalar color, int thickness, int offset) {
	line(mat, Point(t[0], t[1] + offset), Point(t[2], t[3] + offset), color, thickness);
}

vector<Mat> makeGrid(Mat img, int yOff) {
	Mat grd = Mat(img.size(), img.type(), white);
	rectangle(grd, Point(0, yOff), Point(img.cols, 0), lghtGry, -1);
	double roiH = double(img.rows - yOff), roiW = double(img.cols);
	int rSpace = int(roiH/double(6)); 
	int cSpace = int(roiW/8);
	//cout << img.size() << " roiH:" << roiH << " roiW:" << roiW << " rSp:" << rSpace << " cSp:" << cSpace << "\n";
	
	Mat side(Size(cSpace, img.rows), img.type(), lghtGry);
	for(int i = yOff; i < img.rows; i += rSpace) {
		line(grd, Point(0, i), Point(roiW, i), black, 1);
		putText(side, to_string(i), Point(15, i), 4, .4, white);
	}
	for (int i = 0; i < img.cols; i += cSpace) {
		line(grd, Point(i, 0), Point(i, grd.rows), black, 1);
		putText(grd, to_string(i), Point(i, 37), 4, .4, white);
		}
	vector<Mat> grdParts = {grd, side};
	return grdParts;
}
