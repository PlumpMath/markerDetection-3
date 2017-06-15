#include "toPerspec.h"

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "왼쪽 마우스 버튼 클릭.. 좌표 = (" << x << ", " << y << ")" << endl;
		vector<Point2f> * beforeCord = (vector<Point2f>*)userdata;
		beforeCord->push_back(Point2f(x, y));
	}
}

int toPerspec::setInputImg(Mat inputImg) {
	inputImg.copyTo(this->inputImg);
	return 1;
}

int toPerspec::setBfHomoByClick() {
	namedWindow("original image", WINDOW_AUTOSIZE);
	imshow("original image", this->inputImg);
	setMouseCallback("original image", CallBackFunc, &(this->bfHomo));
	waitKey(0);

	return 1;
}

int toPerspec::setAfHomo(vector<Point2f> afHomo) {
	this->afHomo = afHomo;
	return 1;
}

int toPerspec::setSize(float x, float y) {
	this->width = x;
	this->height = y;
	return 1;
}


int toPerspec::Process() {

	Point2f src[4], dst[4];

	src[0] = bfHomo[0];
	src[1] = bfHomo[1];
	src[2] = bfHomo[2];
	src[3] = bfHomo[3];
	dst[0] = afHomo[0];
	dst[1] = afHomo[1];
	dst[2] = afHomo[2];
	dst[3] = afHomo[3];

	transMat = getPerspectiveTransform(src, dst);
	warpPerspective(this->inputImg, this->outputImg, this->transMat, Size(this->width, this->height));
	return 1;
}

Mat toPerspec::getInputImg() {
	return this->inputImg;
}

vector<Point2f> toPerspec::getBfHomo() {
	return this->bfHomo;
}


vector<Point2f> toPerspec::getAfHomo() {
	return this->afHomo;
}

Mat toPerspec::getOutputImg() {
	return this->outputImg;
}


