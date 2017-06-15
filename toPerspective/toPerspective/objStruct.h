#pragma once
#ifndef OBJSTRUCT_H
#define OBJSTRUCT_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

struct obj{
	int camIdx = 0;
	int id = -1;
	Point2f pos;
	obj* next;
};

#endif // !OBJSTRUCT_H
