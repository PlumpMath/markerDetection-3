
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/core.hpp>
#include <cmath>

#include <fstream>


using namespace std;
using namespace cv;

char key;

namespace cv {
	// calculates the median value of a single channel
	// based on https://github.com/arnaudgelas/OpenCVExamples/blob/master/cvMat/Statistics/Median/Median.cpp
	double median(cv::Mat channel)
	{
		double m = (channel.rows*channel.cols) / 2;
		int bin = 0;
		double med = -1.0;

		int histSize = 256;
		float range[] = { 0, 256 };
		const float* histRange = { range };
		bool uniform = true;
		bool accumulate = false;
		cv::Mat hist;
		cv::calcHist(&channel, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

		for (int i = 0; i < histSize && med < 0.0; ++i)
		{
			bin += cvRound(hist.at< float >(i));
			if (bin > m && med < 0.0)
				med = i;
		}

		return med;
	}

	double median(cv::Mat channel, Mat mask)
	{
		double m = countNonZero(mask) / 2;
		int bin = 0;
		double med = -1.0;

		int histSize = 256;
		float range[] = { 0, 256 };
		const float* histRange = { range };
		bool uniform = true;
		bool accumulate = false;
		cv::Mat hist;
		cv::calcHist(&channel, 1, 0, mask, hist, 1, &histSize, &histRange, uniform, accumulate);

		for (int i = 0; i < histSize && med < 0.0; ++i)
		{
			bin += cvRound(hist.at< float >(i));
			if (bin > m && med < 0.0)
				med = i;
		}

		return med;
	}

	double mode(cv::Mat channel)
	{
		//double m = countNonZero(mask) / 2;
		int bin = 0;
		double mod = -1.0;

		int histSize = 256;
		float range[] = { 0, 256 };
		const float* histRange = { range };
		bool uniform = true;
		bool accumulate = false;
		cv::Mat hist;
		cv::calcHist(&channel, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

		float maxPixelValue = -1;
		float maxPixelValueFrequncy = -1;

		for (int i = 0; i < histSize; ++i)
		{
			if (maxPixelValueFrequncy < cvRound(hist.at< float >(i))) {
				maxPixelValueFrequncy = cvRound(hist.at< float >(i));
				maxPixelValue = i;

			}
		}

		return maxPixelValue;
	}

	double mode(cv::Mat channel, Mat mask)
	{
		double m = countNonZero(mask) / 2;
		int bin = 0;
		double mod = -1.0;

		int histSize = 256;
		float range[] = { 0, 256 };
		const float* histRange = { range };
		bool uniform = true;
		bool accumulate = false;
		cv::Mat hist;
		cv::calcHist(&channel, 1, 0, mask, hist, 1, &histSize, &histRange, uniform, accumulate);

		float maxPixelValue = -1;
		float maxPixelValueFrequncy = -1;

		for (int i = 0; i < histSize; ++i)
		{
			if (maxPixelValueFrequncy < cvRound(hist.at< float >(i))) {
				maxPixelValueFrequncy = cvRound(hist.at< float >(i));
				maxPixelValue = i;

			}
		}

		return maxPixelValue;
	}


}


double externalPoint(int m, int n, double v1, double v2) {
	return (m*v2 - n*v1) / (m - n);
}


using namespace std;


int main(int argc, char *argv[]) {

	Mat image, imageCopy, imageGray;

	//there are 3 ways to obtain image  (1) static image file  (2) video file  (3) camera

	//(1) static image file   
	//image = imread("17.jpg", CV_LOAD_IMAGE_COLOR);
	//imshow("image", image);
	//waitKey(0);

	//(2) video file
	VideoCapture inputVideo("frames.avi");

	//(3) camera
	//VideoCapture inputVideo(0); // open the default camera

	if (!inputVideo.isOpened())  // check if we succeeded
		return -1;


	vector<Point2f> afterPer;
	vector<Point2f> beforePer;

	////for static image
	//beforePer.push_back(cv::Point2f(1527, 1001));
	//beforePer.push_back(cv::Point2f(1767, 55));
	//beforePer.push_back(cv::Point2f(68, 246));
	//beforePer.push_back(cv::Point2f(582, 1030));

	//for video
	//beforePer.push_back(cv::Point2f(993, 651));
	//beforePer.push_back(cv::Point2f(1277, 6));
	//beforePer.push_back(cv::Point2f(30, 54));
	//beforePer.push_back(cv::Point2f(341, 644));

	beforePer.push_back(cv::Point2f(285, 8));
	beforePer.push_back(cv::Point2f(45, 677));
	beforePer.push_back(cv::Point2f(1254, 619));
	beforePer.push_back(cv::Point2f(948, 7));


	double afterPer_length = 500; //마커 6x6크기일때 검은색 테두리 영역 포함한 크기는 8x8
								  //이후 단계에서 이미지를 격자로 분할할 시 셀하나의 픽셀너비를 10으로 한다면
								  //마커 이미지의 한변 길이는 80
	afterPer.push_back(cv::Point2f(0, 0));
	afterPer.push_back(cv::Point2f(0, afterPer_length - 1));
	afterPer.push_back(cv::Point2f(afterPer_length - 1, afterPer_length - 1));
	afterPer.push_back(cv::Point2f(afterPer_length - 1, 0));



	//perspective
	//마커를 사각형형태로 바꿀 perspective transformation matrix를 구한다.
	Mat PerspectiveTransformMatrix = getPerspectiveTransform(beforePer, afterPer);


	//resize(image, image, Size(), 0.5, 0.5);


	double totalTime = 0;
	double findContoursTime = 0;
	double findRectTime = 0;
	double findWhiteEdgeTime = 0;
	double findDirTime = 0;
	double findColorTime = 0;

	int totalIterations = 0;

	int frameNum = 0;

	Point2f pastPos[4] = { Point2f(0,0), };
	bool isFirstFrame[4] = { 1,1,1,1 };

	ofstream outFile("color distribution.txt");

	while (inputVideo.grab()) {

		frameNum++;


		//if (frameNum - 1 < 148) { continue; }
		inputVideo.retrieve(image);

		cout << endl;
		cout << endl;
		cout << "******************************** frameNum : " << frameNum << endl;
		//if (frameNum < 800) {
		//	continue;
		//}

		//perspective transformation을 적용한다. 
		warpPerspective(image, image, PerspectiveTransformMatrix, Size(afterPer_length, afterPer_length));

		double tick = (double)getTickCount();

		//imshow("perspec", image);
		//waitKey(1);

		//Mat afterBS;

		//getBS(image, afterBS, hsv_lower, hsv_upper, rgb_lower, rgb_upper);


		image.copyTo(imageCopy);
		cvtColor(image, imageGray, CV_BGR2GRAY);


		//Mat Source = imread("S1.jpg", 1);
		//Mat mMask = imread("Smask.bmp", 0); This is your Create mask output.
		//	Mat Destination;
		//Source.copyTo(Destination, mMask);
		//imshow("Destination", Destination);



		Mat binary_image;
		//		for(int i = 0; i<100; i++){
		//			cout << 3 + i * 2 << endl;
		//threshold(imageGray, binary_image, 200, 255, THRESH_BINARY | THRESH_OTSU);

		adaptiveThreshold(imageGray, binary_image, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 15, 7);
		//			adaptiveThreshold(imageGray, binary_image, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 3 + i * 2, 7);
//					adaptiveThreshold(imageGray, binary_image, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 25, 3 + i*2);

		imshow("binary_image", binary_image);
		waitKey(1);
		//		}


				//contours를 찾는다.
		Mat contour_image = binary_image.clone();
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		findContours(contour_image, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
		//		findContours(contour_image, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);



		double currentfindContoursTime = ((double)getTickCount() - tick) / getTickFrequency();
		findContoursTime += currentfindContoursTime;

		/// Draw contour

		//Mat forContourshow = imageCopy.clone();
		//for (int i = 0; i < contours.size(); i++)
		//{
		//	drawContours(forContourshow, contours, i, Scalar(0, 255, 0), 1, LINE_AA);
		//}
		//
		//imshow("Contours", forContourshow);
		//waitKey(1);

		//contour를 근사화한다.
		vector<vector<Point2f>> marker;
		vector<vector<int>> dist;
		vector<Point2f> approx;

		//		vector<vector<Point2f> > colCorVec;
		//		vector<Point2f> colCorPoi;
		Mat forRectshow;
		imageCopy.copyTo(forRectshow);

		for (size_t i = 0; i < contours.size(); i++)
		{
			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.05, true);

			if (
				(approx.size() == 4) &&
				fabs(contourArea(Mat(approx))) > 95 && //면적이 일정크기 이상이어야 한다. 300
				fabs(contourArea(Mat(approx))) < 140 && //면적이 일정크기 이하여야 한다. 600
				isContourConvex(Mat(approx)) //convex인지 검사한다.
				 //사각형은 4개의 vertex를 가진다. 
				)
			{
				//draw appoxpoly
				drawContours(forRectshow, contours, i, Scalar(0, 0, 255), 1, LINE_AA);

				//circle(imageCopy, approx[0], 5, Scalar(255, 0, 0), CV_FILLED);

				cout << "fabs(contourArea(Mat(approx))): " << fabs(contourArea(Mat(approx))) << endl;

				//String markerId = to_string(marker.size());
				//putText(imageCopy, markerId, Point(contours[i][0].x, contours[i][0].y), 1, 2, Scalar(0, 255, 0), 2, 8);


				vector<cv::Point2f> points;
				for (int j = 0; j < 4; j++)
					points.push_back(cv::Point2f(approx[j].x, approx[j].y));

				//반시계 방향으로 정렬
				cv::Point v1 = points[1] - points[0];
				cv::Point v2 = points[2] - points[0];

				double o = (v1.x * v2.y) - (v1.y * v2.x);
				if (o < 0.0)
					swap(points[1], points[3]);


				//짧은 변이 먼저 들어감
				int dis0to1, dis1to2, dist2to3;
				dis0to1 = sqrt(pow(points[0].x - points[1].x, 2) + pow(points[0].y - points[1].y, 2));
				dis1to2 = sqrt(pow(points[1].x - points[2].x, 2) + pow(points[1].y - points[2].y, 2));
				if (dis0to1 > dis1to2) {
					swap(points[1], points[2]);
					swap(points[0], points[3]);
					swap(points[1], points[3]);
					swap(dis0to1, dis1to2);
				}

				dist2to3 = sqrt(pow(points[2].x - points[3].x, 2) + pow(points[2].y - points[3].y, 2));


				//circle(imageCopy, points[0], 3, Scalar(255, 0, 0), CV_FILLED);
				//circle(imageCopy, points[1], 3, Scalar(255, 0, 0), CV_FILLED);
				vector<int> distSet;
				distSet.push_back(dis0to1);
				distSet.push_back(dis1to2);
				//cout << dis0to1 << " " << dis1to2 << endl;
				dist.push_back(distSet);

				marker.push_back(points);

				//circle(imageCopy, colCorPoi[0], 3, Scalar(255, 255, 0), CV_FILLED);

			}



		}
		//cout << "Rectangle : " << marker.size() << endl;
		//namedWindow("Rectangle", WINDOW_NORMAL);
		//resizeWindow("Rectangle", imageCopy.cols/2, imageCopy.rows / 2);

		string RectangleSize = "Rectangle : " + to_string(marker.size());
		putText(forRectshow, RectangleSize, Point(forRectshow.cols*0.01, forRectshow.rows*0.05), 2, 1, Scalar(0, 0, 255), 1, 8);

		imshow("Rectangle", forRectshow);
		//if (frameNum >= 109) {
		//	waitKey(0);
		//}
		waitKey(1);

		//if (marker.size() < 4) {
		//	
		//	//imshow("Rectangle", imageCopy);
		//	//waitKey();

		//}



		double currentfindRectTime = ((double)getTickCount() - tick) / getTickFrequency();
		findRectTime += currentfindRectTime;


		/////////////////////////////////////////////////////////////////
		Mat forWhiteEdgeshow;
		imageCopy.copyTo(forWhiteEdgeshow);


		vector<vector<Point2f> > RectWhiteEdge;
		vector<Mat>  RectWhiteEdgeImage;


		Mat marker_image;
		//	for (int i = 3; i < 4; i++)
		//	for (int i = 9; i < 10; i++)
		for (int i = 0; i < marker.size(); i++)
		{
			//cout << i << endl;

			vector<Point2f> square_points;
			//double marker_image_side_length;
			double marker_image_side_x = dist[i][0] * 10;
			double marker_image_side_y = dist[i][1] * 10;

			square_points.push_back(cv::Point2f(0, 0));
			square_points.push_back(cv::Point2f(marker_image_side_x - 1, 0));
			square_points.push_back(cv::Point2f(marker_image_side_x - 1, marker_image_side_y - 1));
			square_points.push_back(cv::Point2f(0, marker_image_side_y - 1));

			//cout << "marker index : " << i << endl;
			vector<Point2f> m = marker[i];
			Mat BlackEdgeRaw;

			//perspective
			//마커를 사각형형태로 바꿀 perspective transformation matrix를 구한다.
			Mat PerspectiveTransformMatrix = getPerspectiveTransform(m, square_points);
			//perspective transformation을 적용한다. 
			warpPerspective(imageGray, marker_image, PerspectiveTransformMatrix, Size(marker_image_side_x, marker_image_side_y));

			//for dibugging
			warpPerspective(image, BlackEdgeRaw, PerspectiveTransformMatrix, Size(marker_image_side_x, marker_image_side_y));


			/////////////////////////////////////////////////

			//threshold(marker_image, marker_image, 125, 255, THRESH_BINARY | THRESH_OTSU);

			////if (i == 0) {
			////	imshow("marker_image0", BlackEdgeRaw);
			////	waitKey(1);

			////	imshow("binary0", marker_image);
			////	waitKey(1);
			////}
			////else if (i == 1) {
			////	imshow("marker_image1", BlackEdgeRaw);
			////	waitKey(1);

			////	imshow("binary1", marker_image);
			////	waitKey(1);
			////}
			////else if (i == 2) {
			////	imshow("marker_image2", BlackEdgeRaw);
			////	waitKey(1);

			////	imshow("binary2", marker_image);
			////	waitKey(1);
			////}
			////else if (i == 3) {
			////	imshow("marker_image3", BlackEdgeRaw);
			////	waitKey(1);

			////	imshow("binary3", marker_image);
			////	waitKey(1);
			////}
			////else if (i == 4) {
			////	imshow("marker_image4", BlackEdgeRaw);
			////	waitKey(1);

			////	imshow("binary4", marker_image);
			////	waitKey(1);
			////}
			////else if (i == 5) {
			////	imshow("marker_image5", BlackEdgeRaw);
			////	waitKey(1);

			////	imshow("binary5", marker_image);
			////	waitKey(1);
			////}
			////else if (i == 6) {
			////	imshow("marker_image6", BlackEdgeRaw);
			////	waitKey(1);

			////	imshow("binary6", marker_image);
			////	waitKey(1);
			////}
			////else if (i == 7) {
			////	imshow("marker_image7", BlackEdgeRaw);
			////	waitKey(1);

			////	imshow("binary7", marker_image);
			////	waitKey(1);
			////}

			////destroyWindow("binary");

			//int white_cell_count = 0;
			//int black_cell_count = 0;
			////
			////     ㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁ
			////     ㅁㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅁ
			////     ㅁㅣㅁㅁㅁㅁㅁㅁㅁㅁㅣㅁ
			////     ㅁㅣㅁㅁㅁㅁㅁㅁㅁㅁㅣㅁ
			////     ㅁㅣㅁㅁㅁㅁㅁㅁㅁㅁㅣㅁ
			////     ㅁㅣㅁㅁㅁㅁㅁㅁㅁㅁㅣㅁ
			////     ㅁㅣㅁㅁㅁㅁㅁㅁㅁㅁㅣㅁ
			////     ㅁㅣㅁㅁㅁㅁㅁㅁㅁㅁㅣㅁ
			////     ㅁㅣㅁㅁㅁㅁㅁㅁㅁㅁㅣㅁ
			////     ㅁㅣㅁㅁㅁㅁㅁㅁㅁㅁㅣㅁ
			////     ㅁㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅁ
			////     ㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁ

			////for dibug
			////Mat whiteEdgeCheck(Mat::zeros(marker_image.rows, marker_image.cols, CV_8UC1));
			////cout << BlackEdgeRaw.cols << " " << BlackEdgeRaw.rows << endl;

			//for (int y = round(marker_image_side_y / 90 * 5); y < round(marker_image_side_y / 90 * 85); y++)
			//{
			//	for (int x = round(marker_image_side_x / 65 * 5); x < round(marker_image_side_x / 65 * 60); x++)
			//	{
			//		//cout << x << "   " << y << endl;
			//		if ((round(marker_image_side_y / 90 * 10) <= y) && (y < round(marker_image_side_y / 90 * 80)) && (round(marker_image_side_x / 65 * 10) == x))
			//		{
			//			x = round(marker_image_side_x / 65 * 55) - 1;
			//			continue;
			//		}

			//		//cout << x << ", " << y << endl;

			//		//for dibug
			//		/*whiteEdgeCheck.at<uchar>(y, x) = 255;*/

			//		if ((int)marker_image.at<uchar>(y, x) == 0) {

			//			black_cell_count++;
			//		}

			//		else {
			//			white_cell_count++;
			//		}
			//	}
			//}

			////for dibug
			////imshow("whiteEdgeCheck", whiteEdgeCheck);
			////waitKey();





			////waitKey();
			//cout << "blackCell : " << black_cell_count << " totlaCell : " << black_cell_count + white_cell_count << endl;
			//if (white_cell_count > (black_cell_count + white_cell_count) * 0.5) {
			//	RectWhiteEdge.push_back(m);
			//	//Mat img = marker_image.clone();
			//	//detectedMarkersImage.push_back(img);
			//	RectWhiteEdgeImage.push_back(BlackEdgeRaw);

			//	circle(forWhiteEdgeshow, m[0], 5, Scalar(0, 255, 0), CV_FILLED);
			//	//String markerId = to_string(RectWhiteEdgeImage.size() - 1);
			//	//putText(forWhiteEdgeshow, markerId, Point(m[0].x + 30, m[0].y + 30), 1, 2, Scalar(0, 255, 0), 2, 8);
			//	//putText(imageCopy, "whiteEdge", Point(m[0].x + 30, m[0].y + 40), 1, 1, Scalar(0, 255, 0), 1, 8);

			//	//imshow("white edge", BlackEdgeRaw);
			//	//waitKey();
			//	//destroyWindow("black edge");
			//}
			/////////////////////////////////////////////////


			RectWhiteEdge.push_back(m);
			RectWhiteEdgeImage.push_back(BlackEdgeRaw);


			//cout << "marker index : " << i << endl;


//			vector<Point2f> m = marker[i];
//
//			Mat BlackEdgeRaw;
//
//			//perspective
//			//마커를 사각형형태로 바꿀 perspective transformation matrix를 구한다.
//			Mat PerspectiveTransformMatrix = getPerspectiveTransform(m, square_points);
//			//perspective transformation을 적용한다. 
//			warpPerspective(image, BlackEdgeRaw, PerspectiveTransformMatrix, Size(marker_image_side_length, marker_image_side_length));
//			warpPerspective(imageGray, marker_image, PerspectiveTransformMatrix, Size(marker_image_side_length, marker_image_side_length));
//			detectedMarkers.push_back(m);
//			//Mat img = marker_image.clone();
//			//detectedMarkersImage.push_back(img);
//			detectedMarkersImage.push_back(BlackEdgeRaw);
//
//			//circle(imageCopy, m[0], 10, Scalar(255, 0, 0), CV_FILLED);
//			String markerId = to_string(detectedMarkersImage.size() - 1);
//			putText(imageCopy, markerId, Point(m[0].x + 30, m[0].y + 30), 1, 2, Scalar(0, 255, 0), 2, 8);
////			imshow("whiteEdge", imageCopy);
////			waitKey(0);
//			//destroyWindow("black edge");
		}
		cout << "whiteEdge :" << RectWhiteEdgeImage.size() << endl;
		//namedWindow("whiteEdge", WINDOW_NORMAL);
		//resizeWindow("whiteEdge", imageCopy.cols / 2, imageCopy.rows / 2);
		//imshow("whiteEdge", imageCopy);
		//waitKey(0);

		//string RectWhiteEdgeSize = to_string(RectWhiteEdge.size());
		//putText(imageCopy, "whiteEdge", Point(imageCopy.cols*0.8, imageCopy.rows*0.15), 1, 2, Scalar(0, 255, 0), 2, 8);
		//putText(imageCopy, RectWhiteEdgeSize, Point(imageCopy.cols*0.8, imageCopy.rows*0.2), 1, 2, Scalar(0, 255, 0), 2, 8);




		//string whiteEdgeSize = "WhiteEdge : " + to_string(RectWhiteEdge.size());
		//putText(forWhiteEdgeshow, whiteEdgeSize, Point(forWhiteEdgeshow.cols*0.01, forWhiteEdgeshow.rows*0.05), 2, 1, Scalar(0, 255, 0), 1, 8);

		//imshow("white edge", forWhiteEdgeshow);
		//waitKey(1);


		//if (RectWhiteEdge.size() < 4) {
		//	imshow("whiteEdge", imageCopy);
		//	waitKey(0);
		//}

		/*
		if (detectedMarkers.size() != 2) {
			cout << "black Edge.size() : " << detectedMarkers.size() << endl;
			waitKey();
		}
		*/



		double currentfindBlackEdgeTime = ((double)getTickCount() - tick) / getTickFrequency();
		findWhiteEdgeTime += currentfindBlackEdgeTime;

		//////
		//cout << "detectedMarker Size : " << detectedMarkers.size() << endl;

		////////////



		//*raw perspec
		Mat marker_image_raw;

		//vector<vector<int>> marker_code;


		//vector<vector<point2f>> crrect_marker_code;

		//	for (int markerIdx = 1; markerIdx < 2; markerIdx++)

		///////////////////////////////////////////////////////////////////

		vector<int> dir;
		vector<vector<Point2f>> RectHaveCorrectDir;
		vector<Mat>RectHaveCorrectDirImage;

		Mat forDirshow;
		imageCopy.copyTo(forDirshow);

		cout << endl;
		// RectWhiteEdge에 대하여 내부에 적절한 외곽선이 있는지 확인
		for (int markerIdx = 0; markerIdx < RectWhiteEdge.size(); markerIdx++)
			//		for (int markerIdx = 7; markerIdx < 12; markerIdx++)
		{
			//cout << endl;
			//cout << markerIdx << "번째 RectWhiteEdge" << endl;


			//top, bottom 자르기
			vector<Point2f> m = RectWhiteEdge[markerIdx];

			Mat src = RectWhiteEdgeImage[markerIdx];

			Mat srcGray, scrBinary;
			cvtColor(src, srcGray, CV_BGR2GRAY);
			//adaptiveThreshold(srcGray, scrBinary, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 31, 7);
			threshold(srcGray, scrBinary, 125, 255, THRESH_BINARY | THRESH_OTSU);

			//Mat top, bottom;
			Rect topRect(round(src.cols * 15 / 65), round(src.rows * 12 / 92), round(src.cols * 35 / 65), round(src.rows * 16 / 92));


			////for debug

			Mat srcCopy;
			scrBinary.copyTo(srcCopy);
			rectangle(srcCopy, topRect, Scalar(255, 0, 0), 1);
			/*		if (markerIdx == 0) {
						imshow("topRect0", srcCopy);
						waitKey(1);
					}else if (markerIdx == 1) {
						imshow("topRect1", srcCopy);
						waitKey(1);
					}
					else if (markerIdx == 2) {
						imshow("topRect2", srcCopy);
						waitKey(1);
					}
					else if (markerIdx == 3) {
						imshow("topRect3", srcCopy);
						waitKey(1);
					}
					else if (markerIdx == 4) {
						imshow("topRect4", srcCopy);
						waitKey(1);
					}
					else if (markerIdx == 5) {
						imshow("topRect5", srcCopy);
						waitKey(1);
					}
					else if (markerIdx ==6) {
						imshow("topRect6", srcCopy);
						waitKey(1);
					}
					else if (markerIdx ==7) {
						imshow("topRect7", srcCopy);
						waitKey(1);
					}*/

					//for debug
			Mat top(scrBinary, topRect);
			Mat flipSrcBinary;
			flip(scrBinary, flipSrcBinary, -1);
			Mat bottom(flipSrcBinary, topRect);

			//adaptiveThreshold(top, top, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 31, 7);
			//adaptiveThreshold(bottom, bottom, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 31, 7);
			threshold(top, top, 125, 255, THRESH_BINARY | THRESH_OTSU);
			threshold(bottom, bottom, 125, 255, THRESH_BINARY | THRESH_OTSU);

			int topNumWhite, bottomNumWhite;
			topNumWhite = countNonZero(top);
			bottomNumWhite = countNonZero(bottom);

			int tempDir = 0;

			cout << "topNumWhite : " << topNumWhite << ", " << "bottomNumWhite : " << bottomNumWhite << endl;
			if (topNumWhite > bottomNumWhite) {
				tempDir = 0;
				circle(forDirshow, m[0], 5, Scalar(255, 255, 0), CV_FILLED);
			}
			else if (bottomNumWhite> topNumWhite) {
				tempDir = 2;
				circle(forDirshow, m[2], 5, Scalar(255, 255, 0), CV_FILLED);
			}
			else {
				continue;
			}

			RectHaveCorrectDir.push_back(RectWhiteEdge[markerIdx]);
			RectHaveCorrectDirImage.push_back(RectWhiteEdgeImage[markerIdx]);
			dir.push_back(tempDir);
		}

//		cout << "RectHaveCorrectDir : " << RectHaveCorrectDir.size() << endl;


		string RectHaveCorrectDirSize = "Dir : " + to_string(RectHaveCorrectDir.size());
		putText(forDirshow, RectHaveCorrectDirSize, Point(forDirshow.cols*0.01, forDirshow.rows*0.05), 2, 1, Scalar(0, 0, 255), 1, 8);

		imshow("Dir", forDirshow);
		waitKey(1);

		//if (RectHaveCorrectDir.size() < 4) {
		//	imshow("RectHaveCorrectDir", imageCopy);
		//	waitKey();
		//}

		double currentFindDirTime = ((double)getTickCount() - tick) / getTickFrequency();
		findDirTime += currentFindDirTime;

		vector<Mat> finalMarkerImage;
		vector < vector<Point2f>> finalMarker;
		vector <int> markerColor;
		vector <int> markerDir;

		Mat forColorshow;
		imageCopy.copyTo(forColorshow);

		//색 검출
		/////////////////////////////////////////////////////////////////////
		outFile << RectHaveCorrectDir.size() << " ";

		for (int markerIdx = 0; markerIdx < RectHaveCorrectDir.size(); markerIdx++)
		{
			cout << endl;
			cout << markerIdx << "번째 RectHaveCorrectDir" << endl;

			vector<Point2f> m = RectWhiteEdge[markerIdx];

			Mat src = RectHaveCorrectDirImage[markerIdx];

			Rect roi(round(src.cols * 15 / 65), round(src.rows * 30 / 92), round(src.cols * 35 / 65), round(src.rows * 30 / 92));


			//for debug
			//Mat srcCopy;
			//src.copyTo(srcCopy);
			//rectangle(srcCopy, roi, Scalar(255, 0, 0), 1);
			//imshow("topRect", srcCopy);
			//waitKey();

			Mat colorRect(src, roi);

			Mat HSV;
			cvtColor(colorRect, HSV, CV_BGR2HSV);
			vector<Mat> channels;
			split(HSV, channels);

			Scalar Median1 = median(channels[0]);
			Scalar Median2 = median(channels[1]);
			Scalar Median3 = median(channels[2]);

			cout << " median : " << (int)Median1[0] << "  ";
			cout << (int)Median2[0] << "  ";
			cout << (int)Median3[0] << endl;

			Scalar Mode1 = mode(channels[0]);
			Scalar Mode2 = mode(channels[1]);
			Scalar Mode3 = mode(channels[2]);

			cout << " mode : " << (int)Mode1[0] << "  ";
			cout << (int)Mode2[0] << "  ";
			cout << (int)Mode3[0] << endl;
			////
			////// 각 영역에서 색 검출하기


			outFile <<(int)Mode1[0] << " "<< (int)Mode2[0] << " "<< (int)Mode3[0] << " " << (int)Median1[0] << " " << (int)Median2[0] << " " << (int)Median3[0] << " / ";


			int tempColor;
			//if ((160 < Mode1[0] && Mode1[0] < 180) || (-1 <= (int)Mode1[0] && Mode1[0] <= 10)&& Mode3[0]>115) { //red
			//	tempColor = 0;
			//}//sure
			if (((160 <= Median1[0] && Median1[0] <= 180) || (0 <= Median1[0] && Median1[0] <= 20)) && Median3[0]>100) { //red
				tempColor = 0;
			}
			else if (25 < Median1[0] && Median1[0] < 45 && Median3[0]>180) { //yellow
				tempColor = 3;
			}//sure
			else if ((45 < Median1[0] && Median1[0] < 105) && Median3[0]>30 ) { //green
				tempColor = 1;
			}
			else if ((105 <= Median1[0] && Median1[0] < 130)) { //blue
				if ((int)Median2[0] < 50 && (int)Median3[0] < 135) {
					tempColor = 1;
				}
				else{
					tempColor = 2;
				}

				//tempColor = 2;
			}
			//else if ((155 < Median1[0] && Median1[0] < 180) || (0 <= Median1[0] && Median1[0] <= 25)) { //red
			//	tempColor = 0;
			//}
			else {
				cout << "no matching color" << endl;;
				cout << " mode : " << (int)Mode1[0] << "  ";
				cout << (int)Mode2[0] << "  ";

				cout << (int)Mode3[0] << endl;
				continue;
			}

			cout << "tempColor : " << tempColor << endl;


			finalMarker.push_back(RectHaveCorrectDir[markerIdx]);
			finalMarkerImage.push_back(RectHaveCorrectDirImage[markerIdx]);
			markerDir.push_back(dir[markerIdx]);
			markerColor.push_back(tempColor);
		}

		outFile << endl;

		cout << "finalMarker : " << finalMarker.size() << endl;

		double currentfindMarkerTime = ((double)getTickCount() - tick) / getTickFrequency();
		findColorTime += currentfindMarkerTime;


		Mat pastPosCalculatedMarkerImage[4];
		Point2f pastPosCalculatedMarker[4];
		Point2f topPos[4];
		Point2f bottomPos[4];
		int markerFinalDir[4];

		bool isChecked[4] = { 0, };
		float tempDist[4] = { 0, };

		for (int i = 0; i < finalMarker.size(); i++) {
			//cout << "correct marker size : " << crrect_marker_code.size() << endl;

			vector<Point2f> m = finalMarker[i];

			int id = markerColor[i];

			//not R,G,B,Y
			//if (id > 3) { continue; }

			int leftTop = markerDir[i];
			int rightTop = (leftTop + 1) % 4;
			int rigthBottom = (leftTop + 2) % 4;
			int leftBottom = (leftTop + 3) % 4;

			Point2f TopMiddle((m[leftTop].x + m[rightTop].x) / 2, (m[leftTop].y + m[rightTop].y) / 2);
			Point2f BottomMiddle((m[leftBottom].x + m[rigthBottom].x) / 2, (m[leftBottom].y + m[rigthBottom].y) / 2);
			Point2f center((TopMiddle.x + BottomMiddle.x) / 2, (TopMiddle.y + BottomMiddle.y) / 2);

			// compare with past postion

			if (isFirstFrame[id]) {
				isFirstFrame[id] = 0;
			}
			else {
				Point2f diff = center - pastPos[id];
				float dist = sqrt(diff.x*diff.x + diff.y*diff.y);

				if (isChecked[id]) {
					if (tempDist[id] < dist) {
						cout << id <<" : 나보다 더 가까운 차가 있다" << endl;
						continue;
					}
				}
				else if (dist > 100) {
					cout << "너무 멀다" << endl;
					continue;
				}
				tempDist[id] = dist;
			}

			isChecked[id] = 1;
			pastPos[id] = center;


			pastPosCalculatedMarker[id] = center;
			topPos[id] = TopMiddle;
			bottomPos[id] = BottomMiddle;
			pastPosCalculatedMarkerImage[id] = finalMarkerImage[i];
			markerFinalDir[id] = dir[i];
		}


		////그림 그리기
		Mat finalMarkerImg = image.clone();
		int finalSize = 0;

		for (int i = 0; i < 4; i++) {
			if (!isChecked[i]) {
				continue;
			}

			finalSize++;
			//		drawContours(finalMarkerImg, crrect_marker_code, i, Scalar(0, 255, 0), 1, LINE_AA);
			//circle(finalMarkerImg, m[0], 3, Scalar(255, 0, 0), CV_FILLED);

			String markerId = to_string(i);

			arrowedLine(finalMarkerImg, topPos[i], bottomPos[i], Scalar(255, 0, 255), 2, 8, 0, 0.5);
			putText(finalMarkerImg, markerId, Point(topPos[i].x + 20, topPos[i].y + 20), 2, 1, Scalar(0, 0, 255), 1, 8);

		}


		//putText(finalMarkerImg, "red 0    green 1    blue 2    yellow 3", Point(finalMarkerImg.cols*0.3, finalMarkerImg.rows*0.3), 2, 1, Scalar(0, 0, 0), 1, 8);
		string final = "final : " + to_string(finalSize);
		putText(finalMarkerImg, final, Point(forDirshow.cols*0.01, forDirshow.rows*0.05), 2, 1, Scalar(0, 0, 255), 1, 8);

		imshow("final", finalMarkerImg);
		if (4 != finalSize) {
			waitKey(0);
		}
		waitKey(1);


		//imshow("finalMarkerImg", finalMarkerImg);
		//waitKey(1);


		double currentTime = ((double)getTickCount() - tick) / getTickFrequency();
		totalTime += currentTime;
		totalIterations++;
		if (totalIterations % 30 == 0) {
			cout << "Detection Time = " << currentTime * 1000 << " ms "
				<< "(Mean = " << 1000 * totalTime / double(totalIterations) << " ms)" << endl;
			cout << "findContoursTime = " << "(Mean = " << 1000 * findContoursTime / double(totalIterations) << " ms)" << endl;
			cout << "findRectTime = " << "(Mean = " << 1000 * findRectTime / double(totalIterations) - 1000 * findContoursTime / double(totalIterations) << " ms)" << endl;
			cout << "findBlackEdgeTime = " << "(Mean = " << 1000 * findWhiteEdgeTime / double(totalIterations) - 1000 * findRectTime / double(totalIterations) << " ms)" << endl;
			cout << "findMarkerTime = " << "(Mean = " << 1000 * findColorTime / double(totalIterations) - 1000 * findWhiteEdgeTime / double(totalIterations) << " ms)" << endl;
			//waitKey();
		}
		//		/*
		//		if (crrect_marker_code.size() != 2 || (id[0] != 0 && id[1] != 0) || (id[0] != 3 && id[1] != 3) )
		//		{
		//			cout << "crrect_marker_code.size() " << crrect_marker_code.size() << endl;
		//			waitKey();
		//		}
		//		*/
		//		
		//
		//		//imshow("final", finalMarkerImg);
		//		
		//		//waitKey();
		//		//waitKey();
		//
		////	}
		//
		//	//sort(crrect_marker_code.begin(), crrect_marker_code.end())
		//
		//	//}
	}

	outFile.close();

	return 0;

}