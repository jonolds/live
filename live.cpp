#include <opencv2/opencv.hpp>
#include <fstream>
#include "VidCont.hpp"

using namespace std; using namespace cv;

int main() {
	//VidCont vid("../media/test10.mp4", "../media/outVid.avi");
	VidCont vid("../media/test24.mp4");
	//VidCont vid;
	vid.run();
	return 0;
}