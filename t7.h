#ifndef T7_H
#define T7_H
#include <opencv2/opencv.hpp>

using namespace std;

class t7 : public cv::Vec4i
{
public:
	double m, ang;
	int b;
	//char set;

	t7();
	t7(int, int, int, int);
	explicit t7(cv::Vec4i);

	void init(int v0, int v1, int v2, int v3);
	double getAng();
	void setAng(double Ang);
	int getB();
	void setB(int B);
	double getM();
	void setM(double M);
	//char getSet();					void setSet(char c);

	cv::Point p1();
	cv::Point p2();
	cv::Point intrPt(t7 ln2);
	cv::Point ptAtX(int x);
	cv::Point ptAtY(int y);
	int xAtY(int y);
	int yAtX(int x);
	int vanX(t7 ln2);
	int vanY(t7 ln2);
};

#endif
