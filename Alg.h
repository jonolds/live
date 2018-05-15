#ifndef ALG_H
#define  ALG_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
#include <math.h>
#include <iomanip>
#include <numeric>

using namespace std; using namespace cv; using namespace viz;
class Alg {
public:
	Vec4i gAveLine, rAveLine;
	vector<Vec4i> farSlopeVec;
	deque<double> angleSums;
	int y_offset, frameCountAlg = 0, hThresh = 30, minAngle, maxAngle;
	double lowThr, highThr, minLen, maxGap, resizeFactor = .35, offsetFactor = .2;
	bool l2 = false, test = false;
	Point topLeftP, topMidLeftP, topMidRightP, topRightP, topMidP, leftSideP, rightSideP, botLeftP, botRightP;
	Point gTopPt, gBotPt, rTopPt, rBotPt;
	Alg() {}; ~Alg() { cvDestroyAllWindows(); }
	Mat init(Mat src, Mat&  out), applyCanny(Mat src_gray);
	void process(Mat& src, Mat& out), applyHoughP(Mat& im_edge, Mat& out, vector<Vec4i>& lines);
	void sortLines(Mat& img, vector<Vec4i>& lines, vector<Vec4i>& greens, vector<Vec4i>& reds);
	void drawLines(Mat& out, vector<Vec4i> greens, vector<Vec4i> reds);
	void drawMarks(Mat& out);
};
#endif