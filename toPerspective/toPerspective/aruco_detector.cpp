#include "aruco_detector.h"

aruco_detector::~aruco_detector() {
	delete ids, positions;
}

void aruco_detector::aruco_detector_initialize(int camId, int dictionaryId, bool showRejected, Mat* inputIm){
	this->camId = camId;
	this->dictionaryId = dictionaryId;
	this->showRejected = showRejected;
	this->inputIm = inputIm;
	this->dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
	this->detectorParams = aruco::DetectorParameters::create();
	detectorParams->doCornerRefinement = true; // do corner refinement in markers

	vector<int>* ids;
	vector<Point2f>* positions;
	this->ids = new vector<int>;
	this->positions = new vector<Point2f>;
};

void aruco_detector::process() {
	Mat image, imageCopy;
	image = *inputIm;

	vector< int > ids;
	vector< vector< Point2f > > corners, rejected;
	vector< Vec3d > rvecs, tvecs;

	// detect markers and estimate pose
	aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);

	Point2f sum(0, 0);
	for (int i = 0; i < ids.size(); i++) {
		sum.x = 0;
		sum.y = 0;
		for (int j = 0; j < 4; j++) {
			sum = sum+ corners[i][j];
		}
		this->positions->push_back(sum/4);
//		this->positions->push_back(sum / 4);
		this->ids->push_back(ids[i]);

	}

	// draw results
	image.copyTo(imageCopy);
	if (ids.size() > 0) {
		aruco::drawDetectedMarkers(imageCopy, corners, ids);
	}

	if (showRejected && rejected.size() > 0)
		aruco::drawDetectedMarkers(imageCopy, rejected, noArray(), Scalar(100, 0, 255));

	imshow("out", imageCopy);
	waitKey(0);
}

vector<int>* aruco_detector::get_ids() {
	return this->ids;
}
vector<Point2f>* aruco_detector::get_poses() {
	return this->positions;
}