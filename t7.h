#ifndef T7_H
#define T7_H
#include <opencv2/opencv.hpp>

class t7 : public cv::Vec4i {
public:
	double m, ang;
	int b;

	t7();
	t7(int, int, int, int);
	explicit t7(cv::Vec4i);
	void init();

	cv::Point p1(), p2();
	cv::Point intrPt(t7 ln2);
	cv::Point ptAtX(int x);
	cv::Point ptAtY(int y);
	int xAtY(int y);
	int yAtX(int x);
	int vanX(t7 ln2);
	int vanY(t7 ln2);
};

#endif
