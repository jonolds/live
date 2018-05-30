#ifndef T8LN_HPP
#define T8LN_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <cmath>
#include <numeric>
#include <iostream>

using namespace std;
using namespace cv;

class t7ln {
public:
	Vec4i v4;
	double m, ang;
	int b;
	char set;

	t7ln() {}
	explicit t7ln(Vec4i v4ln);

	double getAng();				void setAng(double Ang);
	int getB();						void setB(int B);
	double getM();					void setM(double M);
	char getSet();					void setSet(char c);
	Vec4i getV4();					void setV4(Vec4i v);

	Point getP1();					Point getP2();
	Point intersectPt(t7ln ln2);
	Point ptAtX(int x);				Point ptAtY(int y);
	int xAtY(int y);				int yAtX(int x);
	int& operator[](const int n);

};
inline ostream& operator<<(ostream& os, const t7ln& t) {
	os << "v4: " << t.v4 << "  m: " << t.m << "  b: " << t.b << "  ang: " << t.ang << "  set: " << t.set;
	return os;
}
inline t7ln::t7ln(Vec4i v4ln) {
	v4 = v4ln;
	m = (v4ln[2] - v4ln[0] != 0) ? double(v4ln[3] - v4ln[1]) / double(v4ln[2] - v4ln[0]) : 999.0;
	ang = (m != 999) ? (atan(m) * 180) / CV_PI : 999.0;
	b = v4ln[1] - int(m*v4ln[0]);
	set = (m < 0) ? 'g' : 'r';
}

inline double t7ln::getAng() {
	return ang;
}
inline void t7ln::setAng(double Ang) {
	ang = Ang;
}
inline int t7ln::getB() {
	return b;
}
inline void t7ln::setB(int B) {
	b = B;
}
inline double t7ln::getM() {
	return m;
}
inline void t7ln::setM(double M) {
	m = M;
}
inline char t7ln::getSet() {
	return set;
}
inline void t7ln::setSet(char c) {
	set = c;
}
inline Vec4i t7ln::getV4() {
	return v4;
}
inline void t7ln::setV4(Vec4i v) {
	v4 = v;
}

inline Point t7ln::getP1() {
	return Point(v4[0], v4[1]);
}
inline Point t7ln::getP2() {
	return Point(v4[2], v4[3]);
}

inline Point t7ln::intersectPt(t7ln ln2) {
	int x = int((ln2.b - b) / (m - ln2.m));
	return Point(x, yAtX(x));
}
inline Point t7ln::ptAtX(int x) {
	return Point(x, yAtX(x));
}
inline Point t7ln::ptAtY(int y) {
	return Point(xAtY(y), y);
}
inline int t7ln::xAtY(int y) {
	return int((y - b) / m);
}
inline int t7ln::yAtX(int x) {
	return int(m*x) + b;
}
inline int& t7ln::operator[](const int n) {
	return v4[n];
}
#endif