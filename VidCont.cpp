#include "VidCont.h"
#include <string>

using namespace std;
using namespace cv;

void VidCont::on_frame(int, void*) {
	
}

VidCont::VidCont(std::string inVidPath, std::string outVidPath) {
	outVid = outVidPath;							//Save out video path
	namedWindow("control", WINDOW_AUTOSIZE);		//create control window
	mod = new Alg();								//create Alg
	reader.release();								//Release reader in case it exists
	
	if (!inVidPath.empty()) {						//open file if one was defined
		reader.open(inVidPath);	
		if (!reader.isOpened()) {
			cout << "Bad file read\n";
			reader.release();
		}
		//fps = int(reader.get(CAP_PROP_FPS));
	}
	
	else {											//open camera if no file defined
		reader.open(0);
		if (!reader.isOpened()) {
			cout << "Cam not open\n";
			reader.release();
		}
		setSize(reader, Size(1280, 720));
		reader.set(CAP_PROP_FPS, fps);
	}
	totalFrames = reader.get(CAP_PROP_FRAME_COUNT);
	isVidOpen = true;
}

void VidCont::run() {
	
	//sprintf(c_str(control), )
	Mat curFrame, outFrame;
	startWindowThread(); //namedWindow(outWin, WINDOW_AUTOSIZE);
	initWriter();
	reader.set(CAP_PROP_POS_FRAMES, 200);
	while (true) {
		if (reader.get(CAP_PROP_POS_FRAMES) == reader.get(CAP_PROP_FRAME_COUNT)) {
			reader.set(CAP_PROP_POS_FRAMES, 200);
			delete mod;
			mod = new Alg();
		}
		if (!reader.read(curFrame))
			break;
		auto initial = getTickCount();
		mod->process(curFrame);
		frameCount++;
		if (!outVid.empty()) {
			writer.write(outFrame);
		}
		double elapsed = (getTickCount() - initial) / (1000 * getTickFrequency());
		int remaining = int(double(1000 / fps) - elapsed);
		frameCount++;
		//imshow(outWin, outFrame); (remaining > 1) ? waitKey(remaining) : waitKey(1);
	}
	endAll();
}

void VidCont::initWriter() {
	if (!outVid.empty()) {
		writer.release();
		int codec = static_cast<int>(reader.get(CAP_PROP_FOURCC));
		writer.open(outVid, codec, fps, Size(int(.35 * getSize(reader).width), int(.35 * getSize(reader).height)), true);
		if (!writer.isOpened()) {
			cout << "Err opening " << outVid << ")\n";
			writer.release();
		}
		isWrInit = true;
	}
}

void VidCont::endAll() {
	writer.release();
	reader.release();
	cvDestroyAllWindows();
}

VidCont::~VidCont() {
	writer.release();
	reader.release();
	cvDestroyAllWindows();
}

Size VidCont::getSize(VideoCapture vc) {
	return Size(int(vc.get(CAP_PROP_FRAME_WIDTH)), int(vc.get(CAP_PROP_FRAME_HEIGHT)));
}

void VidCont::setSize(VideoCapture reader, Size reqSize) {
	reader.set(CAP_PROP_FRAME_WIDTH, reqSize.width);
	reader.set(CAP_PROP_FRAME_HEIGHT, reqSize.height);
}
