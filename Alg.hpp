#ifndef _ALG_H_
#define  _ALG_H_
#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
#include <math.h>
#include <iomanip>
using namespace std; using namespace cv; using namespace viz;
class Alg {
public:
	vector<Vec4i> farSlopeVec;
	int y_offset, frameCountAlg = 0, greenCount = 0, redCount = 0, houghThresh = 30;
	double lowThresh = 50, highThresh = 150, minLength = 30, maxGap = 20, resizeFactor = .35, offsetFactor = .2;
	bool l2gradient = false, test = false;
	Point topLeftP, topMidLeftP, topMidRightP, topRightP, topMidP, leftSideP, rightSideP, botLeftP, botRightP; 
	Alg();
	~Alg();
	void setCanny(double low, double high, bool l2g), setHough(int hThresh, double minLen, double gapMax);
	Mat initMats(Mat src, Mat&  out), applyCanny(Mat src_gray);
	void printLineInf(vector<Vec4i> greens, vector<Vec4i> reds, vector<Vec4i> gFrameLines, vector<Vec4i> rFrameLines, 
		Scalar gMean, Scalar rMean, Scalar gStdDev, Scalar rStdDev);
	void process(Mat& src, Mat& out);
	void applyHoughP(Mat& im_edge, Mat& out, vector<Vec4i>& lines);
	void sortLines(Mat& img, vector<Vec4i>& lines, vector<Vec4i>& greens, vector<Vec4i>& reds);
	void drawLines(Mat& out, vector<Vec4i> greens, vector<Vec4i> reds);
	void drawOrAndYel(Mat& out);
	void boxWrite(Mat& mat, String str, Point pt);
	Vec4i gAveLine, rAveLine;
	void drawMarks(Mat& out);
	deque<double> slopeSums;
	string decStr(double num);
};

void Alg::process(Mat& src, Mat& out) {
	gAveLine = Vec4i::all(0); rAveLine = Vec4i::all(0);
	vector<Vec4i> lines, greens, reds, greenFrameLines, redFrameLines;
	setCanny(30, 105, false); setHough(15, 20, 70);
	Mat src_gray = initMats(src, out);
	y_offset = int(offsetFactor * out.rows);

	Mat edges = applyCanny(src_gray);
	applyHoughP(edges, out, lines);
	
	sortLines(out, lines, greens, reds);
	drawLines(out, greens, reds);
	drawMarks(out);
	//drawOrAndYel(out);
}

Mat Alg::applyCanny(Mat src_gray) {
	blur(src_gray, src_gray, Size(5, 5));
	GaussianBlur(src_gray, src_gray, Size(7, 7), 4);
	GaussianBlur(src_gray, src_gray, Size(5, 7), 4);
	Mat edges(src_gray.size(), CV_8U, Scalar::all(0));
	Canny(src_gray, edges, lowThresh, highThresh, 3, l2gradient);
	
	Point triangle[3] = {Point(int(.1*edges.cols), edges.rows), Point(int(.9*edges.cols), edges.rows), Point(int(.5*edges.cols), y_offset)};
	fillConvexPoly(edges, triangle, 3, Scalar::all(255));
	test = !test;

	namedWindow("canny", WINDOW_GUI_EXPANDED);
	imshow("canny", edges);

	fillConvexPoly(edges, triangle, 3, Scalar::all(0));
	return edges;
}

void Alg::applyHoughP(Mat& im_edge, Mat& out, vector<Vec4i>& lines) {
	lines.clear();
	Rect rect(0, y_offset, im_edge.cols, im_edge.rows - y_offset); // Rect(x,y,w,h) w->width=cols;h->rows
	Mat roi = im_edge(rect);
	HoughLinesP(roi, lines, 1, 1 * CV_PI / 180, houghThresh, minLength, maxGap); //1st best{30, 20, 20}>{30,10,20}>{40, 20, 10} 
	line(out, Point(0, y_offset), Point(out.size().width, y_offset), Color::pink(), 1, 8);
}

void Alg::sortLines(Mat& img, vector<Vec4i>& lines, vector<Vec4i>& greens, vector<Vec4i>& reds) {
	Point maxGreen(0, img.size().height), maxRed(0, img.size().height);
	for (size_t i = 0; i < lines.size(); i++) {
		lines[i][1] += y_offset, lines[i][3] += y_offset;
		int x1 = lines[i][0], y1 = lines[i][1], x2 = lines[i][2], y2 = lines[i][3];
		double slope = x2-x1 != 0 ? double(y2-y1)/double(x2-x1) : 99;
		double angle = slope != 99 ? (atan(slope) * 180) / CV_PI : 99.0;
		if ((abs(angle) > 20)&&(abs(angle) < 62)) {
			if (angle > 0) {
				if (x1 > topMidRightP.x && x2 > topMidRightP.x) {
					reds.emplace_back(lines[i]);
					if (y1 < maxRed.y || y2 < maxGreen.y)
						maxRed = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
				}
			}
			else {
				if ((x1 & x2) < topMidLeftP.x && x2 < topMidLeftP.x) {
					greens.emplace_back(lines[i]);
					if (y1 < maxRed.y || y2 < maxGreen.y)
						maxGreen = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
				}
			}
		}
	}
	farSlopeVec.insert(farSlopeVec.begin(), Vec4i(maxGreen.x, maxGreen.y, maxRed.x, maxRed.y));
}

void Alg::drawLines(Mat& out, vector<Vec4i> greens, vector<Vec4i> reds) {
	double gSlope, rSlope;
	for (int i = 0; i < greens.size(); i++) {
		int x1 = greens[i][0], y1 = greens[i][1], x2 = greens[i][2], y2 = greens[i][3];
		gAveLine[0] += x1; gAveLine[1] += y1; gAveLine[2] += x2; gAveLine[3] += y2;
	}
	for (int i = 0; i < reds.size(); i++) {
		int x1 = reds[i][0], y1 = reds[i][1], x2 = reds[i][2], y2 = reds[i][3];
		rAveLine[0] += x1; rAveLine[1] += y1; rAveLine[2] += x2; rAveLine[3] += y2;
	}
	if (greens.size() > 0) {
		int i = int(greens.size());
		gAveLine /= i;
		line(out, Point(gAveLine[0], gAveLine[1]), Point(gAveLine[2], gAveLine[3]), Color::green(), 2, 8);
		gSlope = atan(double(gAveLine[3] - gAveLine[1]) / double(gAveLine[2] - gAveLine[0])) * 180 / CV_PI;
		boxWrite(out, decStr(gSlope), Point(10, 42));
	}
	if (reds.size() > 0) {
		int i = int(reds.size());
		rAveLine /= i;
		line(out, Point(rAveLine[0], rAveLine[1]), Point(rAveLine[2], rAveLine[3]), Color::red(), 2, 8);
		rSlope = atan(double(rAveLine[3] - rAveLine[1]) / double(rAveLine[2] - rAveLine[0])) * 180 / CV_PI;
		boxWrite(out, decStr(rSlope), Point(300, 42));
	}
	if(reds.size() > 0 && greens.size() > 0)
		slopeSums.emplace_front(rSlope + gSlope);
	if (slopeSums.size() == 9) {
		double slopeAve = 0;
		for(int i = 0; i < 9; i++)
			slopeAve += slopeSums.at(i);
		slopeAve /= 9;
		boxWrite(out, decStr(slopeAve), Point(170, 42));
		slopeSums.pop_back();
		if (slopeAve < -5) {
			arrowedLine(out, Point(60, 15), Point(20, 15), Color::black(), 7, FILLED, 0, 0.25);
			arrowedLine(out, Point(60, 15), Point(20, 15), Color::green(), 2, FILLED, 0, 0.25);
		}
		if (slopeAve > 5) {
			arrowedLine(out, Point(out.cols - 60, 15), Point(out.cols - 20, 15), Color::black(), 7, FILLED, 0, 0.25);
			arrowedLine(out, Point(out.cols - 60, 15), Point(out.cols - 20, 15), Color::red(), 2, FILLED, 0, 0.25);
		}
	}
}
void Alg::boxWrite(Mat& mat, String str, Point pt) {
	int bs = 0;
	Size sz = getTextSize(str, 4, .5, 1, &bs);
	rectangle(mat, Point(pt.x, pt.y +2), Point(pt.x + sz.width, pt.y - sz.height - 1), Color::black(), -1, 8);
	putText(mat, str, pt, /*FONT*/ 4, .5, Color::white(), 1, 8);
}

void Alg::drawMarks(Mat& out) {
	y_offset = int(offsetFactor * out.rows);
	Point yelPts[9] = { topMidP = Point(int(.5*out.cols), y_offset),
		botLeftP = Point(0, out.rows),						botRightP = Point(out.cols, out.rows),
		leftSideP = Point(0, int(.34*out.rows)),			rightSideP = Point(out.cols, int(.34*out.rows)),
		topLeftP = Point(int(.23*out.cols), y_offset),		topMidLeftP = Point(int(.45*out.cols), y_offset),
		topMidRightP = Point(int(.55*out.cols), y_offset),	topRightP = Point(int(.77*out.cols), y_offset) };
	for (int i = 0; i < 9; i++)
		circle(out, yelPts[i], 3, Color::yellow(), -1, 8);
}
void Alg::drawOrAndYel(Mat& out) {
	line(out, Point(farSlopeVec[0][0], farSlopeVec[0][1]), Point(farSlopeVec[0][2], farSlopeVec[0][3]), Color::yellow(), 2, LINE_8, 0);
	int num = 6;
	if (num < int(farSlopeVec.size())) {
		farSlopeVec.pop_back();
		Vec4i sumLastSix(farSlopeVec[0][0], farSlopeVec[0][1], farSlopeVec[0][2], farSlopeVec[0][3]);
		for (int i = 1; i < 6; i++) {
			sumLastSix[0] += farSlopeVec[i][0], sumLastSix[1] += farSlopeVec[i][1];
			sumLastSix[2] += farSlopeVec[i][2], sumLastSix[3] += farSlopeVec[i][3];
		}
		Vec4i aveLastSix(int(sumLastSix[0] / 6), int(sumLastSix[1] / 6), int(sumLastSix[2] / 6), int(sumLastSix[3] / 6));
		line(out, Point(aveLastSix[0], aveLastSix[1]), Point(aveLastSix[2], aveLastSix[3]), Color::orange(), 2, LINE_8, 0);
	}
	frameCountAlg++;
}
Mat Alg::initMats(Mat src, Mat& out) {
	resize(src, src, Size(int(resizeFactor*src.size().width), int(resizeFactor*src.size().height)));
	out = src.clone();
	Mat src_gray;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	return src_gray;
}
string Alg::decStr(double num) {
	ostringstream obj;
	if (abs(num) < 1)
		obj << std::setprecision(2) << num;
	else if (abs(num) < 10)
		obj << std::setprecision(3) << num;
	else
		obj << std::setprecision(4) << num;
	return obj.str();
}
void Alg::printLineInf(vector<Vec4i> greens, vector<Vec4i> reds, vector<Vec4i> greenFrameLines, vector<Vec4i> redFrameLines, Scalar greensMean, Scalar redsMean, Scalar greensStdDev, Scalar redsStdDev) {
	cout << "greens:\n" << Mat(greens) << "\n\nGreens pruned:\n" << Mat(greenFrameLines) << "\n";
	cout << "greensMean:\n" << Mat(greensMean) << "\n\ngreensStdDev: \n" << Mat(greensStdDev) << "\n";
	cout << "reds:\n" << Mat(reds) << "\n\nReds pruned:\n" << Mat(redFrameLines) << "\n";
	cout << "redsMean:\n" << Mat(redsMean) << "\n\nredsStdDev: \n" << Mat(redsStdDev) << "\n";
	cout << "greenCount: " << Mat(greenFrameLines).rows << "   redCount: " << Mat(redFrameLines).rows << "\n";
	cout << "Total greenCount: " << greenCount << "   Total redCount: " << redCount << "\n\n";
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
#endif