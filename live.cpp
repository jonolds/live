#include <opencv2/opencv.hpp>
#include <fstream>
#include "VidCont.hpp"

using namespace std;
using namespace cv;

int main() {
	VidCont vid;
	//vid.setupLive();
	vid.setup("../media/test10.mp4");
	vid.run();
	return 0;
}