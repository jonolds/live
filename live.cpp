#include <opencv2/opencv.hpp>
#include "VidCont.h"
#include <string>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;
void makeWindows();

int main() {
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