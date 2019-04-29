#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<algorithm>
using namespace cv;
Mat salt(Mat in, int n) {
	int i, j;
	Mat out = in.clone();
	for (int k = 0; k < n; k++) {
		i = std::rand() % in.rows;
		j = std::rand() % in.cols;
	//	if (in.type() == CV_8UC3) {
			out.at<Vec3b>(i, j)[0] = 255;
			out.at<Vec3b>(i, j)[1] = 255;
			out.at<Vec3b>(i, j)[2] = 255;
	//	}
	}
	return out;
}
void average(Mat& img) {
	int col = img.cols;
	int row = img.rows;
	Mat kernel(3, 3, CV_32F, Scalar(0));
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			kernel.at<float>(i, j) = (float)1 / 9;
		}
	}
	filter2D(img, img, img.depth(), kernel);
}
void robert(Mat& img) {
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
	img = img1 + img2;
	/*for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);
		}
	}*/
}
int main() {
	Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.1.png", CV_LOAD_IMAGE_COLOR);
	imshow("orign", img);
	Mat do1 = salt(img, 1000);
	average(do1);
	imshow("reducesalt_RGB", do1);
	Mat add = img.clone();
	robert(add);
	Mat do2 = add + img;
	imshow("robert_RGB", do2);
	Mat hsv = img.clone();
	cvtColor(img,hsv, CV_BGR2HSV);
	imshow("HSV", hsv);
	Mat hsvdo1 = salt(hsv, 1000);
	average(hsvdo1);
	imshow("reducesalt_HSV", hsvdo1);
	Mat hsvadd = img.clone();
	robert(hsvadd);
	Mat hsvdo2 =hsvadd +hsv;
	imshow("robert_Lab", hsvdo2);
	Mat lab = img.clone();
	cvtColor(img, lab, CV_BGR2Lab);
	imshow("Lab", lab);

	waitKey();
	return 0;
}