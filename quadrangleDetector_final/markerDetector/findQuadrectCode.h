//
//
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>
//#include <iostream>
//#include <opencv2/core.hpp>
//
//using namespace cv;
//using namespace std;
//
//class ColorMarkerDetector {
//public:
//	int setImgRaw(const Mat imgRaw);
//	Mat getImgRaw();
//
//	int tranRawToPerspec(const vector<Point2f> beforePer, const vector<Point2f> afterPer, const int afterPer_length);
//	Mat getImgPerspec();
//
//	int tranPerspecToGray();
//	Mat getImgGray();
//
//	int tranPerspecToBin(const double maxValue, int adaptiveMethod, int thresholdType, int blockSize, double C);
//	Mat getImgBin();
//
//	int findContour();
//	vector<Point> getPoinContour;
//
//	int findRect();
//	int setEachRectImg();
//	vector<Mat> getImgEachRect;
//
//	int findWhiteEdge();
//	vector<Mat> getImgRectNedge();
//
//	int findDir();
//	vector<Mat> getImgRectNedgeNdir();
//	vector<int> getDir();
//
//	int findCol();
//	vector<Mat> imgRectNedgeNdirNcol();
//	vector<int> getCol();
//
//	Mat getDrawingImg();
//
//
//private:
//	Mat imgRaw, imgPerspec, imgGray, imgBin;
//	vector<Mat> imgRect, imgRectNedge, imgRectNedgeNdir, imgRectNedgeNdirNcol;
//	vector <Point2f> PoiContour;
//	vector<vector<Point2f>> PoiRect, PoiRectNedge, PoiRectNedgeNdir, PoiRectNedgeNdirNcol;
//	vector<int> col, dir;
//};
//
//
//int ColorMarkerDetector::setImgRaw(const Mat imgRaw) {
//	this->imgRaw = imgRaw;
//}
//
//int ColorMarkerDetector::tranRawToPerspec(const vector<Point2f> beforePer, const vector<Point2f> afterPer, const int afterPer_length) {
//	Mat PerspectiveTransformMatrix = getPerspectiveTransform(beforePer, afterPer);
//	warpPerspective(this->imgRaw, this->imgPerspec, PerspectiveTransformMatrix, Size(afterPer_length, afterPer_length));
//}
//
//int ColorMarkerDetector::tranPerspecToGray() {
//	cvtColor(this->imgPerspec, this->imgGray, CV_BGR2GRAY);
//}
//
//int ColorMarkerDetector::tranPerspecToBin(const double maxValue, int adaptiveMethod, int thresholdType, int blockSize, double C) {
//	adaptiveThreshold(this->imgGray, this->imgBin, maxValue, adaptiveMethod, thresholdType, blockSize, C);
//}
//
//int ColorMarkerDetector::findContour() {
//
//}
//int ColorMarkerDetector::findRect() {
//
//}
//int ColorMarkerDetector::setEachRectImg() {
//
//}
//int ColorMarkerDetector::findWhiteEdge() {
//
//}
//int ColorMarkerDetector::findDir() {
//
//}
//int ColorMarkerDetector::findCol() {
//
//}
//
//
//
//
//int findCol();
//int imgRectNedgeNdirNcol();
//
//
//
//Mat getImgRaw();
//Mat getImgPerspec();
//Mat getImgGray();
//Mat getImgBin();
//vector<Point> getPoinContour;
//vector<Mat> getImgEachRect;
//vector<Mat> getImgRectNedge();
//vector<Mat> getImgRectNedgeNdir();
//vector<int> getDir();
//vector<int> getCol();
//Mat getDrawingImg();