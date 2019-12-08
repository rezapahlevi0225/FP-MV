#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <iostream>
#include <wiringPi.h>

using namespace cv;
using namespace std;

#define PWM1	12
#define PWM2	13
#define led		0
int speed = 420;// best 420

void belok_kanan()
{
	pwmWrite(PWM1, speed+30);
	pwmWrite(PWM2, 0);
}
void belok_kiri()
{
	pwmWrite(PWM1, 0);
	pwmWrite(PWM2, speed);
}
void lurus()
{
	pwmWrite(PWM1, speed+30);
	pwmWrite(PWM2, speed);
}
void stop()
{
	pwmWrite(PWM1, 0);
	pwmWrite(PWM2, 0);
}
int main()
{
	//USe Wiringpi Setup
	wiringPiSetupGpio();
	//define pin mode
	pinMode (PWM1, PWM_OUTPUT) ;
	pinMode (PWM2, PWM_OUTPUT) ;
	pinMode (led, OUTPUT) ;
	
	namedWindow("Original", cv::WINDOW_AUTOSIZE);
	namedWindow("HSV", cv::WINDOW_AUTOSIZE);
	namedWindow("Dilation Demo", WINDOW_AUTOSIZE);

	VideoCapture cap;  cap.open(0);
	//VideoCapture cap("mvanjay.mp4");
	Mat frame;  cv::Mat gray;
	Mat garis;
	Mat hsv, garis_hsv;
	//var erode dilate
	Mat src, erosion_dst, dilation_dst;

	int luas_blop;
	int batas_belok_kanan = 140;
	int batas_belok_kiri = 100;
	int batas_max_lubang = 400;
	int batas_min_lubang = 200;
	int batas_tidak_ada_lubang = 600;
	bool cond = false;
	bool stop_sementara = false;
	int save_state = 0;
	int ulang = 0;

	for (;;)
	{
		cap >> frame;
		if (frame.empty()) break;
		resize(frame, frame, Size(240, 160), 0, 0, INTER_CUBIC);
		cvtColor(frame, hsv, COLOR_BGR2HSV);
		inRange(hsv, Scalar(40, 20, 0), Scalar(160, 255, 255), garis_hsv);
		//morph program
		Mat element3 = getStructuringElement(MORPH_ELLIPSE,
			Size(2 * 3 + 1, 2 * 3 + 1),
			Point(3, 3));
		erode(garis_hsv, dilation_dst, element3);
		Mat element = getStructuringElement(MORPH_ELLIPSE,
			Size(2 * 5 + 1, 2 * 5 + 1),
			Point(5, 5));
		dilate(dilation_dst, dilation_dst, element);
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
					if (koordinat.x < batas_belok_kanan && koordinat.x > batas_belok_kiri)
					{
						//cout << "lurus" << endl; 
						lurus();
					}
					else if (koordinat.x >= batas_belok_kanan) 
					{
						//cout << "belok kanan" << endl;
						belok_kanan();
						save_state = 0;
					}
					else if (koordinat.x <= batas_belok_kiri) 
					{
						//cout << "belok kiri" << endl;
						belok_kiri();
						save_state = 1;
					}
				}
				else 
					{
						//cout << "LOST" << endl;
						if(save_state == 0) belok_kiri();
						else belok_kanan();
					}
			}
			//Program detek lubang
			luas_blop = contourArea(contours[i], false);
			cout << luas_blop << endl; // debug luas kabel
			if (luas_blop > batas_min_lubang&& luas_blop < batas_max_lubang)
			{
				ulang++;
				if(ulang>10)
				{
					cond = true;
					digitalWrite(led,HIGH);
					cout << "ada lubang" << endl;
					if(stop_sementara == false)
					{
						stop();
						delay(1000);
						stop_sementara = true;
					}
				}
			}
			else if (luas_blop > batas_tidak_ada_lubang)
			{
				ulang = 0;
				cond = false;
				digitalWrite(led,LOW);
				cout << "Tidak ada lubang" << endl;
				stop_sementara = false;
			}
		}
		bitwise_and(frame, drawing, frame);
		imshow("Original", frame);
		imshow("HSV", hsv);
		imshow("Dilation Demo", dilation_dst);
		if (cv::waitKey(33) >= 0) break;
	}
	stop();
	return 0;
}
