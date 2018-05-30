#include "t7.h"

using namespace std;
using namespace cv;


t7::t7() {}

t7::t7(int v0, int v1, int v2, int v3) : cv::Vec4i(v0, v1, v2, v3) {
	init(v0, v1, v2, v3);
}

t7::t7(Vec4i v) : cv::Vec4i(v[0], v[1], v[2], v[3]) {
	init(v[0], v[1], v[2], v[3]);
}

void t7::init(int v0, int v1, int v2, int v3) {
	m = v2 - v0 != 0 ? double(v3 - v1) / double(v2 - v0) : 999.0;
	ang = m != 999 ? atan(m) * 180 / CV_PI : 999.0;
	b = v1 - int(m * v0);

}

double t7::getAng() {
	return ang;
}

void t7::setAng(double Ang) {
	ang = Ang;
}

int t7::getB() {
	return b;
}

void t7::setB(int B) {
	b = B;
}

double t7::getM() {
	return m;
}

void t7::setM(double M) {
	m = M;
}

Point t7::p1() {
	return Point(val[0], val[1]);
}

Point t7::p2() {
	return Point(val[2], val[3]);
}

Point t7::intrPt(t7 ln2) {
	int x = int((ln2.b - b) / (m - ln2.m));
	return Point(x, yAtX(x));
}

Point t7::ptAtX(int x) {
	return Point(x, yAtX(x));
}

Point t7::ptAtY(int y) {
	return Point(xAtY(y), y);
}

int t7::vanX(t7 ln2) {
	return int((ln2.b - b) / (m - ln2.m));
}

int t7::vanY(t7 ln2) {
	return yAtX(this->vanX(ln2));
}

int t7::xAtY(int y) {
	return int((y - b) / m);
}

int t7::yAtX(int x) {
	return int(m * x) + b;
}
