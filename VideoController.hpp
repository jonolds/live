#ifndef _VIDEOCONTROLLER_H_
#define _VIDEOCONTROLLER_H_
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include "Alg.hpp"
using namespace cv;
using namespace std;

class VideoController {
public:
	VideoCapture vidreader;
	VideoWriter writer;
	Alg* algPtr1; Alg* algPtr2;
	int fps = 20, frameCount = 0;
	Size actualSize, reqSize;
	string outWindowName, outVidName;
	bool stop = false, save = false, isVidOpen = false, isWriterInitialized = false;
	vector<double> frProcTimeVec;
	void initWriter(), endAndReleaseAll(), run(), setupLive(), setSize();
	void setVariables(string oVidName, string oWinName, bool sv, int w, int h);
	VideoController();
	~VideoController();
};
#endif

void VideoController::setupLive() {
	algPtr1 = new Alg(); algPtr2 = new Alg();
	setVariables("outVid.mp4", "live", false, 1280, 720);
	vidreader.release();
	vidreader.open(0);
	setSize();
	if (!vidreader.isOpened()) {
		cout << " Invalid video file read " << endl;
		vidreader.release();
		CV_Assert(false);
	}
	setSize();
	isVidOpen = true;
}

void VideoController::run() {
	CV_Assert(isVidOpen);
	Mat currentFrame, outFrame1, outFrame2;
	startWindowThread();
	namedWindow(outWindowName, WINDOW_AUTOSIZE);
//	namedWindow("outFrame2", WINDOW_AUTOSIZE);
	setSize();
	initWriter();
	while (!stop) {
		// read each frame in video	
		if (!vidreader.read(currentFrame)) // read next frame
			break;
		int initialTime = int(getTickCount());
		algPtr1->process(currentFrame, outFrame1);
		//algPtr2->process2(currentFrame, outFrame2);
		double frameProcessTime = ((double(getTickCount()) - initialTime) / getTickFrequency()) * 1000;
		frProcTimeVec.push_back(frameProcessTime);
		imshow(outWindowName, outFrame1);
//		imshow("outFrame2", outFrame2);
		int elaspedTime = int((getTickCount() - initialTime) / (1000 * getTickFrequency())); //from start of frame read
		int remainingTime = (1000 / fps) - (elaspedTime); //used to prevent early play/process of next frame
		frameCount++;
		(remainingTime > 1) ? waitKey(remainingTime) : waitKey(1);
	}
	endAndReleaseAll();
}
void VideoController::setVariables(string oVidName, string oWinName, bool sv, int w, int h) {
	outVidName = oVidName;
	outWindowName = oWinName;
	save = sv;
	reqSize = Size(w, h);
}
void VideoController::initWriter() {
	if(save){
		writer.release(); // release any previous instance of writer object
		int codec = static_cast<int>(vidreader.get(CAP_PROP_FOURCC));
		writer.open(outVidName, codec, fps, actualSize, true);
		if (!writer.isOpened()) {
			cout << " Error while calling the cv::VideoWriter.open(" << outVidName << ")" << endl;
			writer.release();
			CV_Assert(false);
		}
		isWriterInitialized = true;
	}
}

void VideoController::setSize() {
	vidreader.set(CAP_PROP_FRAME_WIDTH, reqSize.width);
	vidreader.set(CAP_PROP_FRAME_HEIGHT, reqSize.height);
	actualSize = Size(int(vidreader.get(CAP_PROP_FRAME_WIDTH)), int(vidreader.get(CAP_PROP_FRAME_HEIGHT)));
	cout << "\n\nsetSize() -- \nrequested width: " << reqSize.width << "\n   requested height: " << reqSize.height << 
	"\nactual width: " << actualSize.width << "\nactual height: " << actualSize.height << "\n\n";
}
void VideoController::endAndReleaseAll() {
	Scalar m = mean(Mat(frProcTimeVec));
	cout << "\nMean frame proc time " << sum(sum(m)) << "  frames: " << frProcTimeVec.size() << "\n";
	writer.release();
	vidreader.release();
	cvDestroyAllWindows();
}
VideoController::VideoController() {}
VideoController::~VideoController() {
	writer.release();
	vidreader.release();
	cvDestroyAllWindows();
}