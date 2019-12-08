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
	namedWindow("HSV", cv::WINDOW_AUTOSIZE);
	namedWindow("Dilation Demo", WINDOW_AUTOSIZE);

	//VideoCapture cap;  cap.open(0);
	VideoCapture cap("mvanjay.mp4");
	Mat frame;  cv::Mat gray;
	Mat garis;
	Mat hsv, garis_hsv;
	//var erode dilate
	Mat src, erosion_dst, dilation_dst;

	int luas_blop;
	int batas_belok_kanan = 140;
	int batas_belok_kiri = 100;
	int batas_max_lubang = 2130;
	int batas_min_lubang = 2000;
	bool cond = false;

	for (;;)
	{
		cap >> frame;
		if (frame.empty()) break;
		resize(frame, frame, Size(240, 160), 0, 0, INTER_CUBIC);
		cvtColor(frame, hsv, COLOR_BGR2HSV);
		inRange(hsv, Scalar(80, 100, 0), Scalar(120, 255, 255), garis_hsv);
		//morph program
		Mat element = getStructuringElement(MORPH_ELLIPSE,
			Size(2 * 5 + 1, 2 * 5 + 1),
			Point(5, 5));
		dilate(garis_hsv, dilation_dst, element);
		Mat element2 = getStructuringElement(MORPH_ELLIPSE,
			Size(2 * 3 + 1, 2 * 3 + 1),
			Point(3, 3));
		erode(dilation_dst, dilation_dst, element2);

		//program contour
		Mat canny_output;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		/// Find contours
		findContours(dilation_dst, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, Point(0, 0));
		/// Draw contours
		Mat drawing = Mat::zeros(dilation_dst.size(), CV_8UC3);
		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(drawing, contours, i, Scalar(255, 255, 255), FILLED, 8, hierarchy, 0, Point());
			for (unsigned int j = 0;j < contours[i].size(); j++)
			{
				Point koordinat = contours[i][j];
				//cout << "Point(x,y)=" << koordinat << endl; // debug koordinat
				if (koordinat.y < 80)
				{
					if (koordinat.x < batas_belok_kanan && koordinat.x > batas_belok_kiri) cout << "lurus" << endl;
					else if (koordinat.x >= batas_belok_kanan) cout << "belok kanan" << endl;
					else if (koordinat.x <= batas_belok_kiri) cout << "belok kiri" << endl;
					else cout << "LOST" << endl;
				}
			}
			//Program detek lubang
			luas_blop = contourArea(contours[i], false);
			//cout << luas_blop << endl; // debug luas kabel
			if (luas_blop > batas_min_lubang&& luas_blop < batas_max_lubang)
			{
				cond = true;
			}
			else if (luas_blop > batas_max_lubang + 100)
			{
				cond = false;
			}
			if (cond == true) cout << "ada lubang" << endl;
			else cout << "Tidak ada lubang" << endl;
		}
		bitwise_and(frame, drawing, frame);
		imshow("Original", frame);
		imshow("HSV", hsv);
		imshow("Dilation Demo", dilation_dst);
		if (cv::waitKey(33) >= 0) break;
	}
}