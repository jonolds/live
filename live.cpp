#include <opencv2/opencv.hpp>
#include "VidCont.h"
#include <string>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;
void makeWindows();

int main() {
	int framePos = 0;
	double PI = 3.1415926;
	namedWindow("Result", 1);
	VideoCapture cap;
	

	bool camInput = false;
	
	//Cam input
	if(camInput) {
		cap.open(700);
	}
	else {
		cap.open("../media/test24.mp4");
		int totalFrames = (int)cap.get(CAP_PROP_FRAME_COUNT);
		createTrackbar("Position", "Result", &framePos, totalFrames);
	}


	// mat container to receive images
	Mat source, destination;

	// check if capture was successful
	if (!cap.isOpened()) { cout << "cap not open"; return 1; }


	int alpha_ = 90, beta_ = 90, gamma_ = 90;
	int f_ = 500, dist_ = 500;

	
	
	createTrackbar("Alpha", "Result", &alpha_, 180);
	createTrackbar("Beta", "Result", &beta_, 180);
	createTrackbar("Gamma", "Result", &gamma_, 180);
	createTrackbar("f", "Result", &f_, 2000);
	createTrackbar("Distance", "Result", &dist_, 2000);

	while (true) {
		if(!camInput)
			cap.set(CAP_PROP_POS_FRAMES, framePos);
		cap.read(source);
		//resize(source, source, Size(640, 360));

		double alpha = ((double)alpha_ - 90) * PI / 180;
		double beta = ((double)beta_ - 90) * PI / 180;
		double gamma = ((double)gamma_ - 90) * PI / 180;
		double focalLength = (double)f_;
		double dist = (double)dist_;

		Size image_size = source.size();
		double w = (double)image_size.width, h = (double)image_size.height;


		// Projecion matrix 2D -> 3D
		Mat A1 = (Mat_<float>(4, 3) <<
			1, 0, -w / 2,
			0, 1, -h / 2,
			0, 0, 0,
			0, 0, 1);


		// Rotation matrices Rx, Ry, Rz

		Mat RX = (Mat_<float>(4, 4) <<
			1, 0, 0, 0,
			0, cos(alpha), -sin(alpha), 0,
			0, sin(alpha), cos(alpha), 0,
			0, 0, 0, 1);

		Mat RY = (Mat_<float>(4, 4) <<
			cos(beta), 0, -sin(beta), 0,
			0, 1, 0, 0,
			sin(beta), 0, cos(beta), 0,
			0, 0, 0, 1);

		Mat RZ = (Mat_<float>(4, 4) <<
			cos(gamma), -sin(gamma), 0, 0,
			sin(gamma), cos(gamma), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);


		// R - rotation matrix
		Mat R = RX * RY * RZ;

		// T - translation matrix
		Mat T = (Mat_<float>(4, 4) <<
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, dist,
			0, 0, 0, 1);

		// K - intrinsic matrix 
		Mat K = (Mat_<float>(3, 4) <<
			focalLength, 0, w / 2, 0,
			0, focalLength, h / 2, 0,
			0, 0, 1, 0
			);


		Mat transformationMat = K * (T * (R * A1));

		warpPerspective(source, destination, transformationMat, image_size, INTER_CUBIC | WARP_INVERSE_MAP);

		imshow("Result", destination);
		if(waitKey(100) == 27) break;
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