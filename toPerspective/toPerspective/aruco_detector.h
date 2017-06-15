#pragma once
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>

using namespace std;
using namespace cv;

namespace {
	const char* about = "Basic marker detection";
	const char* keys =
		"{d        |       | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
		"DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
		"DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
		"DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
		"{v        |       | Input from video file, if ommited, input comes from camera }"
		"{ci       | 0     | Camera id if input doesnt come from video (-v) }"
		"{c        |       | Camera intrinsic parameters. Needed for camera pose }"
		"{l        | 0.1   | Marker side lenght (in meters). Needed for correct scale in camera pose }"
		"{dp       |       | File of marker detector parameters }"
		"{r        |       | show rejected candidates too }";
}

class aruco_detector {
public:
	aruco_detector() {};
	void aruco_detector_initialize(int camId, int dictionaryId, bool showRejected, Mat* inputIm);
	~aruco_detector();
	void process();
	vector<int>* get_ids();
	vector<Point2f>* get_poses();

private:
	int camId = 0;
	int dictionaryId = 0;
	bool showRejected = false;
	Mat* inputIm;
	Ptr<aruco::Dictionary> dictionary;
	Ptr<aruco::DetectorParameters> detectorParams;

	vector<int>* ids;
	vector<Point2f>* positions;
};