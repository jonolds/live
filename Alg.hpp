#ifndef _ALG_H_
#define  _ALG_H_
#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
#include <math.h>
using namespace std;
using namespace cv;
using namespace viz;

class Alg {
private:
	int y_offset, frameCountAlg = 0, blueCount = 0, redCount = 0;
	vector<Vec4i> farthestSlopeVec;
	double lowThresh = 50;
	double highThresh = 150;
	bool l2gradient = false;
	int houghThresh = 30;
	double minLength = 30;
	double maxGap = 20;
	Point topLeftP, topMidLeftP, topMidRightP, topRightP, topMidP, leftSideP, rightSideP, botLeftP, botRightP;

	double resizeFactor = .22, offsetFactor = .2;
public:
	Alg();
	~Alg();
	void setCanny(double low, double high, bool l2g);
	void setHough(int hThresh, double minLen, double gapMax);
	Mat initMats(Mat src, Mat&  out);
	int getBcount();
	int getRcount();
	void printFrameLineInfo(vector<Vec4i> blues, vector<Vec4i> reds, vector<Vec4i> blueFrameLines,
		vector<Vec4i> redFrameLines,
		Scalar bluesMean, Scalar redsMean, Scalar bluesStdDev, Scalar redsStdDev);

	void process(Mat& src, Mat& out);
	Mat applyCanny(Mat src_gray);
	void applyHoughP(Mat& im_edge, Mat& out, vector<Vec4i>& lines);
	void sortLines(Mat& img, vector<Vec4i>& lines, vector<Vec4i>& blues, vector<Vec4i>& reds);
	void drawLines(Mat& out, vector<Vec4i> blues, vector<Vec4i> reds);
	void drawOrAndYel(Mat& out);
	void drawBlueWarning(Mat& out);
	Vec4i blueAveLine, redAveLine;
	void drawMarks(Mat& out);
};
#endif
void Alg::drawMarks(Mat& out) {
	y_offset = int(offsetFactor * out.rows);
	topLeftP = Point(65, y_offset), topMidLeftP = Point(125, y_offset), topMidRightP = Point(155, y_offset), topRightP = Point(210, y_offset);
	botLeftP = Point(0, out.rows), botRightP = Point(281, out.rows);
	leftSideP = Point(0, 85), rightSideP = Point(out.cols, 85);

	circle(out, topLeftP, 3, Color::yellow(), -1, 8);
	circle(out, topMidLeftP, 3, Color::yellow(), -1, 8);
	circle(out, topMidRightP, 3, Color::yellow(), -1, 8);
	circle(out, topRightP, 3, Color::yellow(), -1, 8);
	circle(out, topMidP, 3, Color::yellow(), -1, 8);
	circle(out, leftSideP, 3, Color::yellow(), -1, 8);
	circle(out, rightSideP, 3, Color::yellow(), -1, 8);
	circle(out, botLeftP, 3, Color::yellow(), -1, 8);
	circle(out, botRightP, 3, Color::yellow(), -1, 8);
}
void Alg::process(Mat& src, Mat& out) {
	blueAveLine = Vec4i(0, 0, 0, 0); redAveLine = Vec4i(0, 0, 0, 0);
	vector<Vec4i> lines, blues, reds, blueFrameLines, redFrameLines;
	setCanny(30, 105, false); setHough(15, 20, 70);
	
	Mat src_gray = initMats(src, out);

	blur(src_gray, src_gray, Size(3, 3));
	blur(src_gray, src_gray, Size(3, 5));
	GaussianBlur(src_gray, src_gray, Size(3, 3), 4);
	
	Mat edges = applyCanny(src_gray);
	applyHoughP(edges, out, lines);
	sortLines(out, lines, blues, reds);
	drawLines(out, blues, reds);
	drawMarks(out);
	//drawOrAndYel(out);
	//drawBlueWarning(out);
}

Mat Alg::applyCanny(Mat src_gray) {
	Mat edges(src_gray.size(), CV_8U, Scalar::all(0));
	Canny(src_gray, edges, lowThresh, highThresh, 3, l2gradient);
	return edges;

}
void Alg::applyHoughP(Mat& im_edge, Mat& out, vector<Vec4i>& lines) {
	lines.clear();
	
	// Rect(x,y,w,h) w->width=cols;h->rows
	Rect rect(0, y_offset, im_edge.cols, im_edge.rows - y_offset);
	Mat roi = im_edge(rect);
	// 1st best {30, 20, 20} > {30,10,20}>{40, 20, 10} 
	HoughLinesP(roi, lines, 1, 1 * CV_PI / 180, houghThresh, minLength, maxGap);
	line(out, Point(0, y_offset), Point(out.size().width, y_offset), Color::pink(), 1, 8);
}

void Alg::sortLines(Mat& img, vector<Vec4i>& lines, vector<Vec4i>& blues, vector<Vec4i>& reds) {
	Point maxBlue(0, img.size().height), maxRed(0, img.size().height);
	for (size_t i = 0; i < lines.size(); i++) {
		lines[i][1] += y_offset, lines[i][3] += y_offset;
		int x1 = lines[i][0], y1 = lines[i][1], x2 = lines[i][2], y2 = lines[i][3];
		double slope = x2-x1 != 0 ? double(y2-y1)/double(x2-x1) : 99;
		double angle = slope != 99 ? (atan(slope) * 180) / CV_PI : 99.0;
		if ((abs(angle) > 20)&&(abs(angle) < 62)) {
			if (angle > 0) {
				if (x1 > topMidRightP.x && x2 > topMidRightP.x) {
					reds.emplace_back(lines[i]);
					if (y1 < maxRed.y || y2 < maxBlue.y)
						maxRed = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
				}
				
			}
			else {
				if (x1 < topMidLeftP.x && x2 < topMidLeftP.x) {
					blues.emplace_back(lines[i]);
					if (y1 < maxRed.y || y2 < maxBlue.y)
						maxBlue = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
				}
				
			}
		}
	}
	farthestSlopeVec.insert(farthestSlopeVec.begin(), Vec4i(maxBlue.x, maxBlue.y, maxRed.x, maxRed.y));
}


void Alg::drawLines(Mat& out, vector<Vec4i> blues, vector<Vec4i> reds) {
	for (int i = 0; i < blues.size(); i++) {
		int x1 = blues[i][0], y1 = blues[i][1], x2 = blues[i][2], y2 = blues[i][3];
		//line(out, Point(x1, y1), Point(x2, y2), Color::blue(), 2, 8);
		blueAveLine[0] += x1; blueAveLine[1] += y1; blueAveLine[2] += x2; blueAveLine[3] += y2;
	}
	for (int i = 0; i < reds.size(); i++) {
		int x1 = reds[i][0], y1 = reds[i][1], x2 = reds[i][2], y2 = reds[i][3];
		//line(out, Point(x1, y1), Point(x2, y2), Color::red(), 2, 8);
		redAveLine[0] += x1; redAveLine[1] += y1; redAveLine[2] += x2; redAveLine[3] += y2;
	}
	if (blues.size() > 1) {
		int i = blues.size();
		blueAveLine /= i;
		/*
		blueAveLine[0] = int(blueAveLine[0] / i); blueAveLine[1] = int(blueAveLine[1] / i);
		blueAveLine[2] = int(blueAveLine[2] / i); blueAveLine[3] = int(blueAveLine[3] / i); */
		line(out, Point(blueAveLine[0], blueAveLine[1]), Point(blueAveLine[2], blueAveLine[3]), Color::green(), 2, 8);
	}
	if (reds.size() > 1) {
		int i = reds.size();
		redAveLine /= i;
		/*
		redAveLine[0] = int(redAveLine[0] / i); redAveLine[1] = int(redAveLine[1] / i);
		redAveLine[2] = int(redAveLine[2] / i); redAveLine[3] = int(redAveLine[3] / i); */
		line(out, Point(redAveLine[0], redAveLine[1]), Point(redAveLine[2], redAveLine[3]), Color::green(), 2, 8);
	}
	
}

void Alg::drawOrAndYel(Mat& out) {
	line(out, Point(farthestSlopeVec[0][0], farthestSlopeVec[0][1]), Point(farthestSlopeVec[0][2], farthestSlopeVec[0][3]),
		Color::yellow(), 2, LINE_8, 0);
	int num = 6;
	if (num < int(farthestSlopeVec.size())) {
		farthestSlopeVec.pop_back();
		Vec4i sumLastSix(farthestSlopeVec[0][0], farthestSlopeVec[0][1], farthestSlopeVec[0][2], farthestSlopeVec[0][3]);
		for (int i = 1; i < 6; i++) {
			sumLastSix[0] += farthestSlopeVec[i][0];
			sumLastSix[1] += farthestSlopeVec[i][1];
			sumLastSix[2] += farthestSlopeVec[i][2];
			sumLastSix[3] += farthestSlopeVec[i][3];
		}
		Vec4i aveLastSix(int(sumLastSix[0] / 6), int(sumLastSix[1] / 6), int(sumLastSix[2] / 6), int(sumLastSix[3] / 6));
		line(out, Point(aveLastSix[0], aveLastSix[1]), Point(aveLastSix[2], aveLastSix[3]), Color::orange(), 2, LINE_8, 0);
	}
	frameCountAlg++;
}

void Alg::drawBlueWarning(Mat& out) {
	arrowedLine(out, Point(100, out.size().height / 2), Point(20, out.size().height / 2), Color::red(), 8, FILLED, 0, 0.25);
	arrowedLine(out, Point(100, out.size().height / 4), Point(20, out.size().height / 4), Color::red(), 8, LINE_AA, 0, 0.25);
}

Mat Alg::initMats(Mat src, Mat& out) {
	
	resize(src, src, Size(int(resizeFactor*src.size().width), int(resizeFactor*src.size().height)));
	out = src.clone();
	Mat src_gray;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	return src_gray;
}

void Alg::printFrameLineInfo(vector<Vec4i> blues, vector<Vec4i> reds, vector<Vec4i> blueFrameLines, vector<Vec4i> redFrameLines, Scalar bluesMean, Scalar redsMean, Scalar bluesStdDev, Scalar redsStdDev) {
	cout << "blues:\n" << Mat(blues) << "\n\nBlues pruned:\n" << Mat(blueFrameLines) << "\n";
	cout << "bluesMean:\n" << Mat(bluesMean) << "\n\nbluesStdDev: \n" << Mat(bluesStdDev) << "\n";
	cout << "reds:\n" << Mat(reds) << "\n\nReds pruned:\n" << Mat(redFrameLines) << "\n";
	cout << "redsMean:\n" << Mat(redsMean) << "\n\nredsStdDev: \n" << Mat(redsStdDev) << "\n";
	cout << "blueCount: " << Mat(blueFrameLines).rows << "   redCount: " << Mat(redFrameLines).rows << "\n";
	cout << "Total blueCount: " << blueCount << "   Total redCount: " << redCount << "\n\n";
}
Alg::Alg() {}
Alg::~Alg() { cvDestroyAllWindows();}
void Alg::setCanny(double low, double high, bool l2g) {
	lowThresh = low;
	highThresh = high;
	l2gradient = l2g;
}
void Alg::setHough(int hThresh, double minLen, double gapMax) {
	houghThresh = hThresh;
	minLength = minLen;
	maxGap = gapMax;
}
int Alg::getBcount() {
	return blueCount;
}
int Alg::getRcount() {
	return redCount;
}
