#ifndef VIDCONT_HPP
#define VIDCONT_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include "Alg.h"
using namespace cv; 
using namespace std;

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
	Size getSize(VideoCapture vc);
	void setSize(VideoCapture reader, Size reqSize);
};

inline VidCont::VidCont(string inVidPath = "", string outVidPath = "") {
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
		reader.set(CAP_PROP_FPS, fps);
	}
	isVidOpen = true;
}

inline void VidCont::run() {
	CV_Assert(isVidOpen);
	Mat curFrame, outFrame;
	startWindowThread(); //namedWindow(outWin, WINDOW_AUTOSIZE);
	initWriter();
	while (true) {
		//if(reader.get(CAP_PROP_POS_FRAMES) == reader.get(CAP_PROP_FRAME_COUNT)) {
		//	reader.set(CAP_PROP_POS_FRAMES, -1);
		//	delete mod;
		//	mod = new Alg();
		//}
		if (!reader.read(curFrame))
			break;
		auto initialTime = getTickCount();
		mod->process(curFrame);	
		frameCount++;
		if(!outVid.empty())
			writer.write(outFrame);
		double elapsedTime = (getTickCount() - initialTime) / (1000 * getTickFrequency());
		double remainingTime = (1000 / fps) - (elapsedTime); //prevents early proc of next frame
		frameCount++;
		//imshow(outWin, outFrame);
		//(remainingTime > 1) ? waitKey(int(remainingTime)) : waitKey(1);  //waitKey();
		//waitKey(1);
	}
	endAll();
}

inline void VidCont::initWriter() {
	if (!outVid.empty()) {
		writer.release();
		int codec = static_cast<int>(reader.get(CAP_PROP_FOURCC));
		writer.open(outVid, codec, fps, Size(int(.35*getSize(reader).width), int(.35*getSize(reader).height)), true);
		if (!writer.isOpened()) { cout << "Err opening " << outVid << ")\n"; writer.release(); CV_Assert(false); }
		isWrInit = true;
	}
}

inline void VidCont::endAll() { writer.release(); reader.release(); cvDestroyAllWindows(); }
inline VidCont::~VidCont() { writer.release(); reader.release(); cvDestroyAllWindows(); }
inline Size VidCont::getSize(VideoCapture vc) {return Size(int(vc.get(CAP_PROP_FRAME_WIDTH)), int(vc.get(CAP_PROP_FRAME_HEIGHT))); }
inline void VidCont::setSize(VideoCapture reader, Size reqSize) {
	reader.set(CAP_PROP_FRAME_WIDTH, reqSize.width);
	reader.set(CAP_PROP_FRAME_HEIGHT, reqSize.height);
}
#endif