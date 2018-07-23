#include <opencv2/opencv.hpp>
#include "VidCont.h"
#include <string>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>



using namespace std;
using namespace cv;
void makeWindows();
int main2(), main3();

int main() {
	main3();
	return 0;
}

int alph_ = 90, bet_ = 90, gam_ = 90, f_ = 500, dist_ = 500;
Mat destination;

void resetVars() {
	alph_ = 90, bet_ = 90, gam_ = 90, f_ = 500, dist_ = 500;
}

void addTrackBars() {
	createTrackbar("Alpha", "Result", &alph_, 180);
	createTrackbar("Beta", "Result", &bet_, 180);
	createTrackbar("Gamma", "Result", &gam_, 180);
	createTrackbar("f", "Result", &f_, 2000);
	createTrackbar("Distance", "Result", &dist_, 2000);
}

Mat getTransMatrix(Size origSz) {
	double PI = 3.1415926;

	double alph = ((double)alph_ - 90) * PI / 180, bet = ((double)bet_ - 90) * PI / 180;
	double gam = ((double)gam_ - 90) * PI / 180, focalLen = (double)f_, dist = (double)dist_;

	double w = (double)origSz.width, h = (double)origSz.height;
	Mat RX = (Mat_<float>(4, 4) << 1, 0, 0, 0, 0, cos(alph), -sin(alph), 0, 0, sin(alph), cos(alph), 0, 0, 0, 0, 1);
	Mat RY = (Mat_<float>(4, 4) << cos(bet), 0, -sin(bet), 0, 0, 1, 0, 0, sin(bet), 0, cos(bet), 0, 0, 0, 0, 1);
	Mat RZ = (Mat_<float>(4, 4) << cos(gam), -sin(gam), 0, 0, sin(gam), cos(gam), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	
	Mat A1 = (Mat_<float>(4, 3) << 1, 0, -w / 2, 0, 1, -h / 2, 0, 0, 0, 0, 0, 1);
	Mat R = RX * RY * RZ;
	Mat T = (Mat_<float>(4, 4) << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, dist, 0, 0, 0, 1);
	Mat K = (Mat_<float>(3, 4) << focalLen, 0, w / 2, 0, 0, focalLen, h / 2, 0, 0, 0, 1, 0);
	return Mat(K * (T * (R * A1)));
}

int main3() {
	//Declare variables
	bool camInput = false;
	VideoCapture cap;
	Mat source;
	int framePos = 0; // totalFrames = 0;
	Size origSz = Size(1280, 720), resizedSz = Size(640, 360);
	
	namedWindow("Result", 1);

	//Cam input 
	if(camInput) {
		cap.open(700);
		if (!cap.isOpened()) { cout << "cap not open"; return 1; }
		cap.set(CAP_PROP_FRAME_WIDTH, origSz.width);
		cap.set(CAP_PROP_FRAME_HEIGHT, origSz.height);
		cap.read(source);
	}
	else {
		cap.open("../media/tape.mp4");
		if (!cap.isOpened()) { cout << "vid not open"; return 1; }
		cap.read(source);
		framePos -= 1;
		createTrackbar("Position", "Result", &framePos, (int)cap.get(CAP_PROP_FRAME_COUNT)-1);
	}
	addTrackBars();
	
	resize(source, destination, resizedSz);
	imshow("Result", destination);
	waitKey(10);

	while (true) {
		cap.set(CAP_PROP_POS_FRAMES, framePos);
		cap.read(source);

		warpPerspective(source, destination, getTransMatrix(origSz), origSz, INTER_CUBIC | WARP_INVERSE_MAP);

		resize(destination, destination, resizedSz);
		imshow("Result", destination);
		char ch = waitKey(10);
		if (ch == 27) 
			break;
		if (ch == 99)
			resetVars();

		
	}
	return 0;
}

int main2() {
	makeWindows();
	//VidCont vid("../media/test24.mp4", "../media/outVid.avi");
	//VidCont vid("../media/tape.mp4");
	VidCont vid;
	vid.run();
	destroyAllWindows();
	return 0;
}

void makeWindows() {
	destroyAllWindows();
	namedWindow("control", WINDOW_FREERATIO);		//create control window
	moveWindow("control", 676, 100);
	namedWindow("inFrame", WINDOW_AUTOSIZE);
	moveWindow("inFrame", 100, 100);
//	namedWindow("outFrame", WINDOW_AUTOSIZE);
//	moveWindow("outFrame", 100, 424);
}