#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <iostream>

using namespace cv;
using namespace std;

void main()
{
	namedWindow("Original", cv::WINDOW_AUTOSIZE);
	namedWindow("Result", cv::WINDOW_AUTOSIZE);
	namedWindow("Trackbar", cv::WINDOW_AUTOSIZE);

	VideoCapture cap;  cap.open(0);
	Mat frame;  cv::Mat gray;
	Mat cdst;
	int slider;
	int Hough_Threshold;

	createTrackbar("Threshold Hough", "Trackbar", &slider, 1000);
	setTrackbarPos("Threshold Hough", "Trackbar", 0);
	for (;;)
	{
		Hough_Threshold = getTrackbarPos("Threshold Hough", "Trackbar");
		cap >> frame;
		if (frame.empty()) break;
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
		int edgeThresh = 1;
		int lowThreshold = 50;
		int const max_lowThreshold = 100;
		int ratio = 3;
		int kernel_size = 3;
		Canny(gray, gray, lowThreshold, lowThreshold * ratio, kernel_size);
		vector<Vec4i> lines;
		HoughLinesP(gray, lines, 1, CV_PI / 180, 28, 20, 10);
	
			for (size_t i = 0; i < lines.size(); i++)
			{
				Vec4i l = lines[i];
				if (l[1] > 300 && l[3] > 300)
				{
				line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, 8, 0);
				if (l[0] < 200) cout << "Belok Kiri" << endl;
				else if (l[0] > 400) cout << "Belok Kanan" << endl;
				else cout << "Lurus" << endl;
				}
			}
		
		imshow("Original", frame);
		imshow("Result", gray);
		if (cv::waitKey(33) >= 0) break;
	}
}
