#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<iostream>
#include<vector>
#include<stdio.h>
#include <math.h>
#include <windows.h>
#define WINVER 0x0500
#define M_PI 3.14159265358979323846

using namespace cv;
using namespace std;
int main(){
	VideoCapture capture(0);
	if (!capture.isOpened()){
		cout << "Camara not online~";
	}
	Size videoSize = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	//namedWindow("show image", CV_WINDOW_AUTOSIZE);

	INPUT ip;
	// Pause for 5 seconds.
	cout << "Wait for five second~~" << endl;
	for (int i = 0; i < 5; i++){
		cout << i + 1 << endl;
		Sleep(1000);
	}
	
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; 
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	

	while (true){
		Mat frame,blur,ycrcb,range_skin,con;
		vector<vector<Point>> contours; // init contours
		//std::vector<std::vector<Point>> hull; // init contours
		capture >> frame;
		if (!frame.empty()){
			
			

			int finger_circle = 0;
			Rect region_of_interest = Rect(320, 120, 310, 330);
			Mat image_roi = frame(region_of_interest);
			rectangle(frame, Point(320, 120), Point(630, 450), Scalar(255, 0, 0),2, 8, 0);
			GaussianBlur(image_roi, blur, Size(5, 5), 2, 0);
			cvtColor(blur, ycrcb, CV_BGR2YCrCb);
			inRange(ycrcb, Scalar(0, 133, 98), Scalar(255, 177, 122), range_skin);

			imshow("Original", frame);
			imshow("cover", range_skin);
			
			//把輪廓弄出來
			findContours(range_skin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //calculate contours

			vector<vector<Point>> hull(contours.size()); // init contours
			vector<vector<int>> hull_int(contours.size());
			vector<Vec4i> hierarchy;
			vector<vector<Vec4i>> defects(contours.size());


			//找hull和convexityDefects
			for (size_t i = 0; i < contours.size(); i++){
				convexHull(Mat(contours[i]), hull[i], false);
				convexHull(Mat(contours[i]), hull_int[i], false);
				if (hull_int[i].size() > 3){
					convexityDefects(Mat(contours[i]), hull_int[i], defects[i]); // You need more than 3 indices
				}
			}


			for (size_t i = 0; i < contours.size(); i++){
				double n = contourArea(contours[i]);
				if (n>5000){
					Rect rect = boundingRect(contours[i]);
					rectangle(image_roi, rect, Scalar(255, 0, 0), 1, 8, 0);
					drawContours(image_roi, contours, i, Scalar(0, 0, 255));
					drawContours(image_roi, hull, i, Scalar(0, 255, 0));
				}
			}

			for (int i = 0; i < contours.size(); ++i){
				for (const Vec4i& v : defects[i]){
					float depth = v[3]/300;
					if (depth > 40) //  filter defects by depth, e.g more than 10
					{
						int startidx = v[0];
						Point ptStart(contours[i][startidx]);
						int endidx = v[1];
						Point ptEnd(contours[i][endidx]);
						int faridx = v[2]; 
						Point ptFar(contours[i][faridx]);

						/*float x = pow((ptEnd.x - ptStart.x), 2);
						float y = pow((ptEnd.y - ptStart.y), 2);
						float a = sqrt(x+y);
						x = pow((ptFar.x - ptStart.x), 2);
						y = pow((ptFar.y - ptStart.y), 2);
						float b = sqrt(x + y);
						x = pow((ptEnd.x - ptFar.x), 2);
						y = pow((ptEnd.y - ptFar.y), 2);
						float c = sqrt(x + y);

						float pa, pb, pc;
						pa = pow(a, 2); pb = pow(b, 2); pc = pow(c, 2);

						float angle = acos((pb+pc-pa) / (2 * b * c));

						if (angle <= M_PI / 2)*/
						

						circle(image_roi, ptFar, 4, Scalar(0, 255, 255), 2);
						circle(image_roi, ptStart, 4, Scalar(255, 0, 255), 2);
						circle(image_roi, ptEnd, 4, Scalar(0, 0, 0), 2);
						finger_circle++;
						//line(image_roi, ptStart, ptEnd, Scalar(0, 255,255), 1);
						//line(image_roi, ptStart, ptFar, Scalar(0, 255, 0), 1);
						//line(image_roi, ptEnd, ptFar, Scalar(0, 255, 0), 1);
						
					}
				}
			}

			//inRange(ycrcb, Scalar(0, 133, 100), Scalar(220, 177, 122), range_skin);
			//cout << finger_circle << endl;
			if (finger_circle >0){
				// Press the "space" key
				ip.ki.wVk = 0x20; // virtual-key code for the "a" key
				ip.ki.dwFlags = 0; // 0 for key press
				SendInput(1, &ip, sizeof(INPUT));
			}
			else{
				// Release the "A" key
				ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
				SendInput(1, &ip, sizeof(INPUT));
			}
			
			//putText(frame, "finger : " + to_string(finger_circle+1), Point(30,30), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0));
			imshow("show image", image_roi);
			
			if (waitKey(33) == 27){
				break;
			}
		}
	}
	return 0;
}
              
void video_capture(){
	VideoCapture capture(0);
	if (!capture.isOpened()){
		cout << "Camara not online~";
	}
	Size videoSize = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	VideoWriter writer;
	writer.open("VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, videoSize);
	namedWindow("show image", 0);

	while (true){
		Mat frame;
		capture >> frame;
		if (!frame.empty()){
			writer.write(frame);
			imshow("show image", frame);
			if (waitKey(33) == 27){
				break;
			}
		}
	}
}
void back_up2(){
	try{
		//convexHull(Mat(contours1[i]), hull[i], false);
	}catch (cv::Exception & e){
		cerr << e.msg << endl; // output exception message
	}
	//看錯誤
}
void backup(){
	int history = 100;
	double learning_rate = -0.45;
	BackgroundSubtractorMOG2 mog = BackgroundSubtractorMOG2(history, 16, true);
	VideoCapture videoOrg("a.avi");
	Mat foreground, img100, gus100, cr100, rang100, hand100;
	for (int i = 0;; i++) {
		Mat frame;
		videoOrg >> frame;
		//mog.operator()(frame, foreground, learning_rate);
		imshow("show image", foreground);
		if (waitKey(33) == 27 || foreground.empty()){
			break;
		}
		if (i == 100){
			imwrite("150_BS.jpg", frame);
			img100 = frame;
			break;
		}
	}
	GaussianBlur(img100, gus100, Size(5, 5), 5, 5);
	imwrite("gus100.jpg", gus100);
	cvtColor(gus100, cr100, CV_BGR2YCrCb);
	imwrite("cr100.jpg", cr100);
	inRange(cr100, Scalar(0, 133, 98), Scalar(255, 177, 122), rang100);
	imwrite("cr100.jpg", rang100);
	img100.copyTo(hand100, rang100);
	imwrite("after_mask.jpg", hand100);
}