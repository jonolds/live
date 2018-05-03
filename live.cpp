#include <opencv2/opencv.hpp>
#include <fstream>
#include "VideoController.hpp"

using namespace std;
using namespace cv;

int main() {
	VideoController vid;
	vid.setupLive();
	vid.run();
	return 0;
}