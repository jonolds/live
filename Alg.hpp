#ifndef _ALG_H_
#define  _ALG_H_
#include <opencv2/opencv.hpp>
#include <opencv2/viz/types.hpp>
#include <vector>
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

	double resizeFactor = .22, offsetFactor = .13;
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
	void setYoffset(int i);

	void process(Mat& src, Mat& out), process2(Mat& src, Mat& out);
	Mat applyCanny(Mat src_gray);
	void applyHoughP(Mat& im_edge, Mat& out, vector<Vec4i>& lines);
	void applyHough(Mat& im_edge, Mat& out, vector<Vec4i>& lines);
	void sortLines(Mat& img, vector<Vec4i>& lines, vector<Vec4i>& blues, vector<Vec4i>& reds);
	void combineLines(vector<Vec4i>& blues, vector<Vec4i>& reds, vector<Vec4i>& blueFrameLines, vector<Vec4i>& redFrameLines);
	void drawLines(Mat& out, vector<Vec4i> blues, vector<Vec4i> reds);
	void drawOrAndYel(Mat& out);
	void drawBlueWarning(Mat& out);
	
};
#endif

void Alg::process(Mat& src, Mat& out) {
	vector<Vec4i> lines, blues, reds, blueFrameLines, redFrameLines;
	setCanny(30, 105, false); setHough(15, 20, 70);
	Mat src_gray = initMats(src, out);

	blur(src_gray, src_gray, Size(3, 3));
	blur(src_gray, src_gray, Size(3, 5));
	GaussianBlur(src_gray, src_gray, Size(3, 3), 4);
	
	Mat edges = applyCanny(src_gray);
	imshow("edges1", edges);
	applyHoughP(edges, out, lines);
	sortLines(out, lines, blues, reds);

	combineLines(blues, reds, blueFrameLines, redFrameLines);
	drawLines(out, blues, reds);
	//drawOrAndYel(out);
	//drawBlueWarning(out);
}
void Alg::process2(Mat& src, Mat& out) {
	vector<Vec4i> lines, blues, reds, blueFrameLines, redFrameLines;
	setCanny(35, 120, false); setHough(25, 20, 65);
	Mat src_gray = initMats(src, out);
	
	GaussianBlur(src_gray, src_gray, Size(5,5), 4);
	//blur(src_gray, src_gray, Size(3, 3));
	Mat edges = applyCanny(src_gray);
	imshow("edges2", edges);
	applyHoughP(edges, out, lines);
	sortLines(out, lines, blues, reds);
	
	combineLines(blues, reds, blueFrameLines, redFrameLines);
	drawLines(out, blues, reds);
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
	y_offset = int(offsetFactor * im_edge.rows);
	// Rect(x,y,w,h) w->width=cols;h->rows
	Rect rect(0, int(y_offset), im_edge.cols, im_edge.rows - int(y_offset));
	Mat roi = im_edge(rect);
	// 1st best {30, 20, 20} > {30,10,20}>{40, 20, 10} 
	HoughLinesP(roi, lines, 1, 1 * CV_PI / 180, houghThresh, minLength, maxGap);
	rectangle(out, rect, Color::pink(), 3, LINE_8, 0);
}

void Alg::applyHough(Mat& im_edge, Mat& out, vector<Vec4i>& lines) {

}


void Alg::sortLines(Mat& img, vector<Vec4i>& lines, vector<Vec4i>& blues, vector<Vec4i>& reds) {
	Point maxBlue(0, img.size().height), maxRed(0, img.size().height);
	for (size_t i = 0; i < lines.size(); i++) {
		lines[i][1] += y_offset, lines[i][3] += y_offset;
		int x1 = lines[i][0], y1 = lines[i][1], x2 = lines[i][2], y2 = lines[i][3];
		double slope = x2 - x1 != 0 ? (tan(double(y2 - y1) / double(x2 - x1)) * 180) / CV_PI : 99.0;
		while (slope > 360) //slope reference to origin at botton left/mathematical orignal
			slope -= 360;
		while (slope < -360)
			slope += 360;
		if (!(abs(abs(slope) - 45) > 20)) {
			if (slope > 0) {
				reds.emplace_back(lines[i]);
				if (y1 < maxRed.y || y2 < maxBlue.y)
					maxRed = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}
			else {
				blues.emplace_back(lines[i]);
				if (y1 < maxRed.y || y2 < maxBlue.y)
					maxBlue = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}
		}
	}
	farthestSlopeVec.insert(farthestSlopeVec.begin(), Vec4i(maxBlue.x, maxBlue.y, maxRed.x, maxRed.y));
}

void Alg::combineLines(vector<Vec4i>& blues, vector<Vec4i>& reds, vector<Vec4i>& blueFrameLines,
	vector<Vec4i>& redFrameLines) {
	Scalar bluesMean, bluesStdDev, redsMean, redsStdDev;
	meanStdDev(blues, bluesMean, bluesStdDev, noArray());
	meanStdDev(reds, redsMean, redsStdDev, noArray());

	for (int i = 0; i < (Mat(blues)).rows; i++) //blues removal
		if (!((abs(blues[i][0] - bluesMean[0]) > bluesStdDev[0]) || (abs(blues[i][2] - bluesMean[2]) > bluesStdDev[2])))
			blueFrameLines.emplace_back(blues[i][0], blues[i][1], blues[i][2], blues[i][3]);
	for (int i = 0; i < Mat(reds).rows; i++) //reds removal
		if (!((abs(reds[i][0] - redsMean[0]) > redsStdDev[0]) || (abs(reds[i][2] - redsMean[2]) > redsStdDev[2])))
			redFrameLines.emplace_back(reds[i][0], reds[i][1], reds[i][2], reds[i][3]);
	blueCount += Mat(blueFrameLines).rows;
	redCount += Mat(redFrameLines).rows;
}

void Alg::drawLines(Mat& out, vector<Vec4i> blues, vector<Vec4i> reds) {
	for (size_t i = 0; i < blues.size(); i++)
		line(out, Point(blues[i][0], blues[i][1]), Point(blues[i][2], blues[i][3]), Color::blue(), 2, 8);
	for (size_t i = 0; i < reds.size(); i++)
		line(out, Point(reds[i][0], reds[i][1]), Point(reds[i][2], reds[i][3]), Color::red(), 2, 8);
}

void Alg::drawOrAndYel(Mat& out) {
	//cout << farthestSlopeVec[0][3] << "\n";
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
void Alg::setYoffset(int i) {
	y_offset = i;
}
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
