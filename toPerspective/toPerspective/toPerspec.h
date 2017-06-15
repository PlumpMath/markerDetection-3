#pragma once
#ifndef TOPERSPEC_H
#define TOPERSPEC_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;

void CallBackFunc(int event, int x, int y, int flags, void* userdata);

class toPerspec {
public:
	int setInputImg(Mat inputImg);
	int setBfHomoByClick();
	int setAfHomo(vector<Point2f> afHomo);
	int setSize(float x, float y);

	int Process();

	Mat getInputImg();
	vector<Point2f> getBfHomo();
	vector<Point2f> getAfHomo();
	Mat getOutputImg();

private:

	vector<Point2f> bfHomo, afHomo;
	float width, height;
	Mat transMat;
	Mat inputImg, outputImg;
};

#endif
