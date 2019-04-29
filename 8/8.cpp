#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stack>
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define WINDOW_NAME "FINAL"
using namespace cv;
using namespace std;

#define ROTATE 1
#define SCALE 2
#define MIRROR 3
#define MAGNIFYING 4
#define ENHANCEMENT 5
#define SEGMENTATION 6
static Point connects[8] = { Point(-1, -1), Point(0, -1), Point(1, -1), Point(1, 0), Point(1, 1), Point(0, 1), Point(-1, 1), Point(-1, 0) }; //定义8领域

Mat dorotate(Mat img, int angle) {
	Mat resultImg(img.size(), img.type());
	Point2f center = Point2f(static_cast<float>(img.cols / 2), static_cast<float>(img.rows / 2));  //to redefine the center point to rotate
	Mat rot = getRotationMatrix2D(center, angle, 1);               //(a,b,c) a refers to the center point to rotate,b refers to the anger and c is the scale rate
	Rect bbox = RotatedRect(center, img.size(), angle).boundingRect(); //RotatedRect returns a rotated matrix and boundingRect returns a smallest rectangle contains all the img
	rot.at<double>(0, 2) += bbox.width / 2.0 - center.x; // use these 2 lines to make sure the img's center is exactly at the center of the new window
	rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;
	Mat dst;
	warpAffine(img, dst, rot, bbox.size());//rot is the matrix to change and bbox.size() is the output size
	return dst;
}
Mat doscale(Mat img, float scale) {
	float scalew = scale;
	float scaleh = scalew;
	Mat out=img.clone();
	int width = static_cast<float>(img.cols*scalew);
	int height = static_cast<float>(img.rows*scaleh);
	resize(img, out, Size(width, height), INTER_NEAREST);
	return out;
}
Mat domirror(Mat img, bool x, bool y) {
		int row = img.rows;
		int col = img.cols;
		Mat mid = img.clone();
		if (y== true) {
			for (int i = 0; i < row; i++) {
				for (int j = 0; j < col; j++) {
					mid.at<uchar>(i, j) = img.at<uchar>(i, col - 1 - j);
				}
			}
		}
		Mat out = mid.clone();
		if (x == true) {
			for (int i = 0; i < row; i++) {
				for (int j = 0; j < col; j++) {
					out.at<uchar>(i, j) =mid.at<uchar>(row - 1 - i, j);
				}
			}
		}
	return out;
}
Mat dorobert(Mat img) {
	int col = img.cols;
	int row = img.rows;
	Mat kernel_x(3, 3, CV_32F, Scalar(0));
	kernel_x.at<float>(1, 1) = -1;
	kernel_x.at<float>(2, 2) = 1;
	Mat img1;
	filter2D(img, img1, img.depth(), kernel_x);
	Mat kernel_y(3, 3, CV_32F, Scalar(0));
	kernel_y.at<float>(1, 2) = -1;
	kernel_y.at<float>(2, 1) = 1;
	Mat img2;
	filter2D(img, img2, img.depth(), kernel_y);
	Mat out = img.clone();
	out= img1 + img2;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			out.at<uchar>(i, j) = 255 - out.at<uchar>(i, j);
		}
	}
	return out;
}
Mat dosobel(Mat img) {
	int col = img.cols;
	int row = img.rows;
	Mat kernel_x(3, 3, CV_32F, Scalar(0));
	kernel_x.at<float>(0, 0) = -1;
	kernel_x.at<float>(0, 1) = -2;
	kernel_x.at<float>(0, 2) = -1;
	kernel_x.at<float>(2, 0) = 1;
	kernel_x.at<float>(2, 1) = 2;
	kernel_x.at<float>(2, 2) = 1;
	Mat img1;
	filter2D(img, img1, img.depth(), kernel_x);
	Mat kernel_y(3, 3, CV_32F, Scalar(0));
	kernel_y.at<float>(0, 0) = -1;
	kernel_y.at<float>(0, 2) = 1;
	kernel_y.at<float>(1, 0) = -2;
	kernel_y.at<float>(1, 2) = 2;
	kernel_y.at<float>(2, 0) = -1;
	kernel_y.at<float>(2, 2) = 1;
	Mat img2;
	filter2D(img, img2, img.depth(), kernel_y);
	Mat out = img.clone();
	out = img1 + img2;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			out.at<uchar>(i, j) = 255 - out.at<uchar>(i, j);
		}
	}
	return out;
}
Mat dothreshold(Mat img, int turn) {  //阈值分割
	Mat out = img.clone();
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img.at<uchar>(i, j) > turn) {
				out.at<uchar>(i, j) = 255;
			}
			else {
				out.at<uchar>(i, j) = 0;
			}
		}
	}
	return out;
}
Mat doregiongrow(Mat in, int x, int y, int limit)   //种子点坐标x,y，阈值为limit进行区域生长算法
{
	if (x > in.rows || y > in.cols) {
		return in;
	}
	Mat out(in.rows, in.cols, CV_8U, Scalar(0));  //生成图像的初始点都是0

	//将种子点入栈 
	stack<Point> pointStack;
	Point seedpoint(x, y);
	pointStack.push(seedpoint);

	//在OutputData中标记生长点为白色
	out.at<uchar>(x, y) = 255;
	int initialColor = in.at<uchar>(x, y);
	Point p;
	int  growLable = 0;//标记是否生长过
	int  inputValue = 0;//待生长点的灰度值
	while (!pointStack.empty())
	{
		//取得 种子点
		Point  topPoint = pointStack.top();
		//从stack中删除该点
		pointStack.pop();
		//遍历该像素的八领域
		for (int i = 0; i < 8; i++)
		{
			p = topPoint + connects[i];//p是待生长点
			int x = p.x;
			int y = p.y;
			//图像的边界检测
			if (x<0 || y<0 || y>(in.cols - 1) || x>(in.rows - 1))
			{
				continue;
			}
			growLable = out.at<uchar>(x, y);
			inputValue = in.at<uchar>(x, y);
			//次点未生长且待生长点与第一个种子点的灰度差的绝对值小于给定值则生长
			if (growLable == 0 && abs(inputValue - initialColor) < limit)
			{
				out.at<uchar>(x, y) = 180;//用灰色表示生长
			   //将此点入栈
				pointStack.push(p);
			}
		}

	}
	return out;
}
int main() {
	cvui::init(WINDOW_NAME);
	Mat frame(500, 1000, CV_8U,Scalar(0));
	bool checkx = false;
	bool checky = false;
	bool robert = false;
	bool sobel = false;
	bool render = true;
	bool isthreshold = false;
	bool isregiongrow = false;
	int mousex = 100;
	int mousey = 100;
	int count = 0;
	int rotate= 0;
	float scale = 1;
	int mirror = 1;
	float magnifying =5;
	int threshold = 50;
	int regiongrow = 50;
	int flag = 0;
	Mat img0= imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.1.png", CV_LOAD_IMAGE_GRAYSCALE); Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img1 = img0.clone();
	int row = img0.rows;
	int col = img0.cols;
	while (true) {
		render = true;
		frame = cv::Scalar(0);
		/*cvui::text(frame, 10, 15, "Hello world!");
		if (cvui::button(frame, 110, 80, "Hello world!")==true) {
			count++;
			cout << "one more click,total:" << count << endl;
		}
		cvui::window(frame, 60, 100, 100, 200, "operator");*/

		cvui::beginColumn(frame,60, 80, 400, 400, 10);
		cvui::text("Function");
		if (cvui::button("Rotate") == 1) {
			cout << "rotate" << endl;
			flag =ROTATE;
		}
		if (flag == ROTATE) {
			cvui::trackbar(150, &rotate, 0, 360);
		}
		if (cvui::button("Scale") == 1) {
			cout << "scale" << endl;
			flag = SCALE;
		}
		if (flag == SCALE) {
			cvui::trackbar(150, &scale, (float)0.5, (float)3);
		}
		if (cvui::button("Mirror") == 1) {
			cout << "morror" << endl;
			flag = MIRROR;
		}
		if (flag == MIRROR) {
			cvui::checkbox( "X", &checkx);
			cvui::checkbox("Y", &checky);
			//cvui::trackbar(150, &mirror, 0, 2);
		}
		if (cvui::button("Magnifying") == 1) {
			cout << "magnifying" << endl;
			flag = MAGNIFYING;
		}
		if (flag == MAGNIFYING) {
			cvui::trackbar(150, &magnifying, (float)2, (float)10);
		}
		if (cvui::button("Enhancement") == 1) {
			cout << "enhancement" << endl;
			flag = ENHANCEMENT;
		}
		if (flag == ENHANCEMENT) {
			cvui::checkbox("robert", &robert);
			if (robert == true && sobel == true) {
				sobel = false;
			}
			cvui::checkbox("sobel", &sobel);
			if (sobel == true && robert== true) {
				robert = false;
			}
		}
		if (cvui::button("Segmentation") == 1) {
			cout << "segmentation" << endl;
			flag = SEGMENTATION;
		}
		if (flag == SEGMENTATION) {
			cvui::checkbox("threshold", &isthreshold);
			if (isthreshold == true && isregiongrow == true) {
				isregiongrow = false;
			}
			cvui::trackbar(150, &threshold, 0, 255);
			cvui::checkbox("region grow", &isregiongrow);
			if (isregiongrow == true && isthreshold == true) {
				isthreshold = false;
			}
			cvui::trackbar(150, &regiongrow, 0, 255);
		}
		cvui::endColumn();

		switch (flag) {
		case ROTATE:
		{
			img1 = dorotate(img0, rotate);
			break;
		}
		case SCALE: {
			Mat img2 = doscale(img0, scale);
			imshow("scale", img2);
			break;
		}
		case MIRROR: {
			img1 = domirror(img0, checkx, checky);
			break;
		}
		case MAGNIFYING: {
			int areacol = col / magnifying;
			int arearow = row / magnifying;
			int areax0 = cvui::mouse().x - 300- areacol / 2;
			int areay0 = cvui::mouse().y - 100-arearow/2;
			if (areax0<0 || areax0+areacol>col || areay0<0 || areay0+arearow>row) {
				render = false;
			}
			if (render == true) {
				Mat mid(arearow, areacol, CV_8U,Scalar(0));
				for (int i = 0; i < mid.rows; i++) {
					for (int j = 0; j < mid.cols; j++) {
						mid.at<uchar>(i, j) = img0.at<uchar>(i + areay0, j + areax0);
					}
				}
				img1 = doscale(mid, magnifying);
			}
		//	cout << areax0 << "," << areay0 << endl;
			break;
		}
		case ENHANCEMENT: {
			if (robert == true) {
				img1 = dorobert(img0);
			}
			else if (sobel == true) {
				img1 = dosobel(img0);
			}
			break;
		}
		case SEGMENTATION: {
			if (isthreshold == true) {
				img1 = dothreshold(img0, threshold);
			}
			else if (isregiongrow == true) {
				img1 =doregiongrow(img0, mousey, mousex, regiongrow);
			}
			break;
		}
		}

		cvui::image(frame, 300, 100, img0);
		cvui::image(frame, 600, 100, img1);
		cvui::text(frame, 380, 400, "Orign", 1);
		cvui::text(frame, 680, 400, "After", 1);
		int status = cvui::iarea(300, 100, col, row);
		switch (status) {
		case cvui::CLICK: 
		{
			cout << "clicked!" << endl;
			break;
		}
		case cvui::DOWN:
		{
			cvui::printf(frame, 25,25, "mouse is: DOWN");
			if (isregiongrow == true) {
				mousex = cvui::mouse().x - 300;
				mousey = cvui::mouse().y - 100;
				cout << mousex << "," << mousey << endl;
			}
			break;
		}
		case cvui::OVER:
		{
			cvui::printf(frame, 25, 25,"mouse is: OVER");
			break;
		}
		case cvui::OUT:
		{
			cvui::printf(frame, 25,25, "mouse is: OUT");
			if (flag == MAGNIFYING) {
				img1 = img0.clone();
			}
			break;
		}
		}
		cvui::imshow(WINDOW_NAME, frame);
		imshow("after", img1);
		if (render == true&&flag!=MAGNIFYING) {
			img1 = img0.clone();
		}
		if (cv::waitKey(30) ==27) {
			break;
		}
	}

	return 0;
}
