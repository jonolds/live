#ifndef T7_HPP
#define T7_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <cmath>
#include <vector>
#include <numeric>
#include <iostream>
#include "colors.h"

using namespace std;
using namespace cv;


class t7 : public Vec4i {
public:
	double m, ang;
	int b;
	//char set;

	t7();
	t7(int, int, int, int);
	explicit t7(Vec4i);

	void init(int v0, int v1, int v2, int v3);
	double getAng();
	void setAng(double Ang);
	int getB();
	void setB(int B);
	double getM();
	void setM(double M);
	//char getSet();					void setSet(char c);

	Point p1();
	Point p2();
	Point intrPt(t7 ln2);
	Point ptAtX(int x);
	Point ptAtY(int y);
	int xAtY(int y);
	int yAtX(int x);
	int vanX(t7 ln2);
	int vanY(t7 ln2);
};
inline t7::t7() {

}
inline t7::t7(int v0, int v1, int v2, int v3) : Vec4i(v0, v1, v2, v3) {
	init(v0, v1, v2, v3);
}

inline t7::t7(Vec4i v) : Vec4i(v[0], v[1], v[2], v[3]) {
	init(v[0], v[1], v[2], v[3]);
}

inline void t7::init(int v0, int v1, int v2, int v3) {
	m = (v2 - v0 != 0) ? double(v3 - v1) / double(v2 - v0) : 999.0;
	ang = (m != 999) ? (atan(m) * 180) / CV_PI : 999.0;
	b = v1 - int(m * v0);
	
}

inline double t7::getAng() {
	return ang;
}

inline void t7::setAng(double Ang) {
	ang = Ang;
}

inline int t7::getB() {
	return b;
}

inline void t7::setB(int B) {
	b = B;
}

inline double t7::getM() {
	return m;
}

inline void t7::setM(double M) {
	m = M;
}

inline Point t7::p1() {
	return Point(val[0], val[1]);
}

inline Point t7::p2() {
	return Point(val[2], val[3]);
}

inline Point t7::intrPt(t7 ln2) {
	int x = int((ln2.b - b) / (m - ln2.m));
	return Point(x, yAtX(x));
}

inline Point t7::ptAtX(int x) {
	return Point(x, yAtX(x));
}

inline Point t7::ptAtY(int y) {
	return Point(xAtY(y), y);
}

inline int t7::vanX(t7 ln2) {
	return int((ln2.b - b) / (m - ln2.m));
}

inline int t7::vanY(t7 ln2) {
	return yAtX(this->vanX(ln2));
}

inline int t7::xAtY(int y) {
	return int((y - b) / m);
}

inline int t7::yAtX(int x) {
	return int(m * x) + b;
}



#endif
