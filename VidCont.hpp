#ifndef _VIDCONT_H_
#define _VIDCONT_H_
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include "Alg.hpp"
using namespace cv; using namespace std;

class VidCont {
public:
	VideoCapture reader;		VideoWriter writer;
	Alg* mod;
	int fps = 25, frameCount = 0;
	Size actualSize, reqSize;
	string outWin, outVid, inVidName = "";
	bool stop = false, isSave = false, isVidOpen = false, isWrInit = false;
	//VidCont member functions
	VidCont();		~VidCont();
	void initWriter(), endAndReleaseAll(), run(), setup(string inVid), setSize();
	void setVariables(string oVidName, string oWinName, bool sv, int w, int h);
};

void VidCont::setup(string inVid = "") {
	mod = new Alg();
	reader.release();
	if (!inVid.empty()) {
		reader.open(inVid);
		cout << reader.get(CAP_PROP_FRAME_WIDTH) << " " << reader.get(CAP_PROP_FRAME_HEIGHT) << "\n";
		if (!reader.isOpened()) { cout << "Bad file read\n"; reader.release(); CV_Assert(false); }
		fps = reader.get(CAP_PROP_FPS);
		setVariables("outVid.mp4", "vid", false, reader.get(CAP_PROP_FRAME_WIDTH), reader.get(CAP_PROP_FRAME_HEIGHT));
	}
	else {
		cout << "live";
		setVariables("outVid.mp4", "live", true, 1280, 720);
		reader.open(0);
		if (!reader.isOpened()) { cout << "Cam not open\n"; reader.release(); CV_Assert(false); }
		setSize();
	}
	isVidOpen = true;
}

void VidCont::run() {
	CV_Assert(isVidOpen);
	Mat currentFrame, outFrame;
	startWindowThread(); namedWindow(outWin, WINDOW_GUI_EXPANDED);
	initWriter();
	while (!stop) {
		if (!reader.read(currentFrame)) // read next frame
			break;
		int initialTime = getTickCount();
		mod->process(currentFrame, outFrame);
		imshow(outWin, outFrame);
		if(isSave)
			writer.write(outFrame);
		int elaspedTime = int((getTickCount() - initialTime) / (1000 * getTickFrequency()));
		int remainingTime = (1000 / fps) - (elaspedTime); //prevents early process of next frame
		frameCount++;
		(remainingTime > 1) ? waitKey(remainingTime) : waitKey(1);  //waitKey();
	}
	endAndReleaseAll();
}
void VidCont::setVariables(string oVidName, string oWinName, bool sv, int w, int h) {
	outVid = oVidName; outWin = oWinName; isSave = sv; reqSize = Size(w, h); }
void VidCont::initWriter() {
	if (isSave) {
		writer.release();
		int codec = static_cast<int>(reader.get(CAP_PROP_FOURCC));
		writer.open(outVid, codec, fps, actualSize, true);
		if (!writer.isOpened()) { cout << "Err opening " << outVid << ")\n"; writer.release(); CV_Assert(false); }
		isWrInit = true;
	}
}

void VidCont::setSize() {
	reader.set(CAP_PROP_FRAME_WIDTH, reqSize.width);
	reader.set(CAP_PROP_FRAME_HEIGHT, reqSize.height);
	actualSize = Size(int(reader.get(CAP_PROP_FRAME_WIDTH)), int(reader.get(CAP_PROP_FRAME_HEIGHT)));
	cout << "\n\nsetSize() -- \nrequested width: " << reqSize.width << "\n   requested height: " << reqSize.height <<
		"\nactual width: " << actualSize.width << "\nactual height: " << actualSize.height << "\n\n";
}
void VidCont::endAndReleaseAll() { writer.release(); reader.release(); cvDestroyAllWindows(); }
VidCont::VidCont() {}
VidCont::~VidCont() { writer.release(); reader.release(); cvDestroyAllWindows(); }
#endif