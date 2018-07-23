#include "VidCont.h"
#include "t7.h"
//#include <string>

using namespace std;
using namespace cv;

void on_frame(int num, void* data) {
	VideoCapture vc = *(static_cast<VideoCapture*>(data));
	vc.set(propPos, num);
	Mat img;
	vc.read(img);
	imshow("control", img);
	waitKey(10);
}

VidCont::VidCont(const string& inVidPath, string outVidPath) {
	outVid = outVidPath;							//Save out video path
	freq = 1000 * getTickFrequency();
	loadCamData();
	reader.release();								//Release reader in case it exists

	if (!inVidPath.empty()) {						//open file if one was defined
		reader.open(inVidPath);
		if (!reader.isOpened()) { cout << "Bad file read\n"; reader.release(); }
		totalFrames = (int)reader.get(propCount);
	}

	else {											//open camera if no file defined
		reader.release();
		reader.open(700);
		if (!reader.isOpened()) { cout << "Cam not open\n"; reader.release(); }
		reader.set(propWidth, 1280);
		reader.set(propHeight, 720);
		reader.set(propFps, fps);
	}
	isVidOpen = true;
	//	initWriter();
}

void onMouse(int evt, int x, int y, int flags, void* param) {
	if (evt == EVENT_LBUTTONDOWN) {
		Point* ptPtr = (Point*)param;
		ptPtr->x = x;
		ptPtr->y = y;
		cout << x << "\n" << y << "\n";
	}
}

void VidCont::run() {
	createTrackbar("Position", "control", &framePos, totalFrames);
	mod = new Alg();

	//createTrackbar("l1X", "control", &l1.x, 448);
	//createTrackbar("l1Y", "control", &l1.y, 252);
	//createTrackbar("l2X", "control", &l2.x, 448);
	//createTrackbar("l2Y", "control", &l2.y, 252);
	//createTrackbar("l3X", "control", &l3.x, 448);
	//createTrackbar("l3Y", "control", &l3.y, 252);
	//createTrackbar("l4X", "control", &l4.x, 448);
	//createTrackbar("l4Y", "control", &l4.y, 252);
	//createTrackbar("r1X", "control", &r1.x, 448);
	//createTrackbar("r1Y", "control", &r1.y, 252);
	//createTrackbar("r2X", "control", &r2.x, 448);
	//createTrackbar("r2Y", "control", &r2.y, 252);
	//createTrackbar("r3X", "control", &r3.x, 448);
	//createTrackbar("r3Y", "control", &r3.y, 252);
	//createTrackbar("r4X", "control", &r4.x, 448);
	//createTrackbar("r4Y", "control", &r4.y, 252);

	Point2i pt(-1, -1);
	setMouseCallback("inFrame", onMouse, (void*)&pt);

	t7 left = t7(133, 63, 45, 108);
	t7 right = t7(281, 63, 380, 108);

	cout << "left bottom x: " << left.ptAtY(252);
	cout << "right bottom x: " << right.ptAtY(252);

	Point2f src_vertices[6] = { Pt(133, 63), Pt(281, 63), Pt(380, 108), Pt(45, 108) };
	//Point2f dst_vertices[6] = { Pt(0,0), Pt(448, 0), Pt(448, 252), Pt(0, 252) };
	Point2f dst_vertices[6] = { Pt(0,0), Pt(448, 0), Pt(448, 108), Pt(0, 108) };
	Mat pTrans = getPerspectiveTransform(src_vertices, dst_vertices);
	while (true) {


		//reader.set(propPos, framePos);
		reader.read(inFrame);

		undistort(inFrame, outFrame, camMat, distCoef);

		resize(inFrame, inFrame, Size(0, 0), .5, .5);
		resize(outFrame, outFrame, Size(0, 0), .5, .5);
		//warpPerspective(inFrame, outFrame, pTrans, outFrame.size());
		//Mat outFrame = mod->process2(inFrame.clone());



		imshow("outFrame", outFrame);
		imshow("inFrame", inFrame);
		if (waitKey(100) == 27)
			break;

		//	undistort(inFrame, outFrame, camMat, distCoef);
		//	transform(inFrame.clone());

		//	drawDots(inFrame);
	}
	cout << "test\n";
}

//while(true) {
//if (framePos == totalFrames){		//loop at last frame
//	framePos = 0;
//	reader.set(propPos, 0);
//	delete mod;						//reset Mod if looping
//	mod = new Alg();
//}
//
//if (!reader.read(inFrame))			//read next frame
//	break;
//framePos++;
//auto initial = getTickCount();		//get current time
//mod->process(inFrame);				//process frame
//if (!outVid.empty())				//write output if out path set
//	writer.write(outFrame);
//
//double elapsed = (getTickCount() - initial) / (freq);	//get elapsed in milliseconds
//int remaining = int(double(1000 / fps) - elapsed);

//imshow(outWin, outFrame); (remaining > 1) ? waitKey(remaining) : waitKey(1);
//}
//}

void VidCont::loadCamData() {
	FileStorage fs("cameraParameters.xml", FileStorage::READ);
	fs["cameraMatrix"] >> camMat;
	fs["dist_coeffs"] >> distCoef;
}

void VidCont::drawDots(Mat& img) {
	circle(img, l1, 2, white, -1); circle(img, r1, 2, white, -1);
	circle(img, l2, 2, white, -1); circle(img, r2, 2, white, -1);
	circle(img, l3, 2, white, -1); circle(img, r3, 2, white, -1);
	circle(img, l4, 2, white, -1); circle(img, r4, 2, white, -1);

	putText(img, "(" + to_string(l1.x) + "," + to_string(l1.y) + ")", l1 + Pt(3, 0), 3, .5, white, 2);
	putText(img, "(" + to_string(l2.x) + "," + to_string(l2.y) + ")", l2 + Pt(3, 0), 3, .5, white, 2);
	putText(img, "(" + to_string(l3.x) + "," + to_string(l3.y) + ")", l3 + Pt(3, 0), 3, .5, white, 2);
	putText(img, "(" + to_string(l4.x) + "," + to_string(l4.y) + ")", l4 + Pt(3, 0), 3, .5, white, 2);

	putText(img, "(" + to_string(r1.x) + "," + to_string(r1.y) + ")", r1 + Pt(3, 0), 3, .5, white, 2);
	putText(img, "(" + to_string(r2.x) + "," + to_string(r2.y) + ")", r2 + Pt(3, 0), 3, .5, white, 2);
	putText(img, "(" + to_string(r3.x) + "," + to_string(r3.y) + ")", r3 + Pt(3, 0), 3, .5, white, 2);
	putText(img, "(" + to_string(r4.x) + "," + to_string(r4.y) + ")", r4 + Pt(3, 0), 3, .5, white, 2);
}

void VidCont::initWriter() {
	if (!outVid.empty()) {
		writer.release();
		int codec = static_cast<int>(reader.get(propFourcc));
		writer.open(outVid, codec, fps, Size(int(.35 * reader.get(propWidth)), int(.35 * reader.get(propHeight))), true);
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
	destroyAllWindows();
}
VidCont::~VidCont() {
	writer.release();
	reader.release();
	destroyAllWindows();
}