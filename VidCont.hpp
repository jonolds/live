#ifndef _VIDCONT_H_
#define _VIDCONT_H_
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include "Alg.hpp"
#include "helper.hpp"
using namespace cv; using namespace std;

class VidCont {
public:
	VideoCapture reader;		VideoWriter writer;
	Alg* mod;
	int fps = 25, frameCount = 0;
	string outVid, outWin = "out";
	bool isVidOpen = false, isWrInit = false;
	//VidCont member functions
	VidCont(string inVidPath, string outVidPath);		~VidCont();
	void initWriter(), endAll(), run();
};
VidCont::VidCont(string inVidPath = "", string outVidPath = "") {
	outVid = outVidPath;
	mod = new Alg();
	reader.release();
	if (!inVidPath.empty()) {
		reader.open(inVidPath);
		if (!reader.isOpened()) { cout << "Bad file read\n"; reader.release(); CV_Assert(false); }
		fps = int(reader.get(CAP_PROP_FPS));
	}
	else {
		reader.open(0);
		if (!reader.isOpened()) { cout << "Cam not open\n"; reader.release(); CV_Assert(false); }
		setSize(reader, Size(1280, 720));
	}
	isVidOpen = true;
}

void VidCont::run() {
	CV_Assert(isVidOpen);
	Mat curFrame, outFrame;
	startWindowThread(); namedWindow(outWin, WINDOW_AUTOSIZE);
	initWriter();
	while (true) {
		if (!reader.read(curFrame))
			break; 
		auto initialTime = getTickCount();
		mod->process(curFrame, outFrame);
		cout << outFrame.size() <<"\n";
		imshow(outWin, outFrame);
		if(!outVid.empty())
			writer.write(outFrame);
		double elapsedTime = (getTickCount() - initialTime) / (1000 * getTickFrequency());
		double remainingTime = (1000 / fps) - (elapsedTime); //prevents early proc of next frame
		frameCount++;
		(remainingTime > 1) ? waitKey(int(remainingTime)) : waitKey(1);  //waitKey();
	}
	endAll();
}
void VidCont::initWriter() {
	if (!outVid.empty()) {
		cout << "balls\n";
		writer.release();
		int codec = static_cast<int>(reader.get(CAP_PROP_FOURCC));
		writer.open(outVid, codec, fps, getSize(reader), true);
		if (!writer.isOpened()) { cout << "Err opening " << outVid << ")\n"; writer.release(); CV_Assert(false); }
		isWrInit = true;
	}
}
void VidCont::endAll() { writer.release(); reader.release(); cvDestroyAllWindows(); }

VidCont::~VidCont() { writer.release(); reader.release(); cvDestroyAllWindows(); }
#endif