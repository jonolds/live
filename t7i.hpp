#ifndef T8LN_HPP
#define T8LN_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <cmath>
#include <numeric>
#include <iostream>

using namespace std;
using namespace cv;

class t7i : public Vec4i {
public:
	double m, ang;
	int b;
	char set;

	t7i() {}
	t7i(int, int, int, int);
	t7i(Vec4i);
	void init(int v0, int v1, int v2, int v3);
	double getAng();				void setAng(double Ang);
	int getB();						void setB(int B);
	double getM();					void setM(double M);
	char getSet();					void setSet(char c);

	Point p1();						Point p2();
	Point intrPt(t7i ln2);
	Point ptAtX(int x);				Point ptAtY(int y);
	int xAtY(int y);				int yAtX(int x);
	int vanX(t7i ln2);				int vanY(t7i ln2);


};

inline t7i::t7i(int v0, int v1, int v2, int v3) : Vec4i(v0, v1, v2, v3) {
	init(v0, v1, v2, v3);
}
inline t7i::t7i(Vec4i v) : Vec4i(v[0], v[1], v[2], v[3]) {
	init(v[0], v[1], v[2], v[3]);
}
inline void t7i::init(int v0, int v1, int v2, int v3) {
	m = (v2 - v0 != 0) ? double(v3 - v1) / double(v2 - v0) : 999.0;
	ang = (m != 999) ? (atan(m) * 180) / CV_PI : 999.0;
	b = v1 - int(m*v0);
	set = (m < 0) ? 'g' : 'r';
}
inline double t7i::getAng() {
	return ang;
}
inline void t7i::setAng(double Ang) {
	ang = Ang;
}
inline int t7i::getB() {
	return b;
}
inline void t7i::setB(int B) {
	b = B;
}
inline double t7i::getM() {
	return m;
}
inline void t7i::setM(double M) {
	m = M;
}
inline char t7i::getSet() {
	return set;
}
inline void t7i::setSet(char c) {
	set = c;
}

inline Point t7i::p1() {
	return Point(val[0], val[1]);
}
inline Point t7i::p2() {
	return Point(val[2], val[3]);
}

inline Point t7i::intrPt(t7i ln2) {
	int x = int((ln2.b - b) / (m - ln2.m));
	return Point(x, yAtX(x));
}
inline Point t7i::ptAtX(int x) {
	return Point(x, yAtX(x));
}
inline Point t7i::ptAtY(int y) {
	return Point(xAtY(y), y);
}
inline int t7i::vanX(t7i ln2) {
	return int((ln2.b - b) / (m - ln2.m));
}
inline int t7i::vanY(t7i ln2) {
	return yAtX(this->vanX(ln2));
}
inline int t7i::xAtY(int y) {
	return int((y - b) / m);
}
inline int t7i::yAtX(int x) {
	return int(m*x) + b;
}
#endif