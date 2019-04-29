#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
using namespace cv;

void log(Mat& output,double a) {  //s=alog(1+r)
	int row = output.rows;
	int col = output.cols;
	for (int i = 0; i< row; i++) {
		for (int j= 0; j< col; j++) {
			int old = output.at<uchar>(i, j);
			double x1 = a * log(1.0 +(double)old);
			if (x1 > 255) {
				x1 = 255;
			}
			else if (x1 < 0) {
				x1 = 0;
			}
			int x2 = static_cast<int>(x1 + 0.5);
			output.at<uchar>(i, j) = x2;
		}
	}
}
void index(Mat& output, double a,double b) {  //s=a*(input^b)
	int row = output.rows;
	int col = output.cols;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int old = output.at<uchar>(i, j);
			double x1 = (a * pow((old) / 255.0,b)) * 255;
			if (x1 > 255) {
				x1 = 255;
			}
			else if (x1 < 0) {
				x1 = 0;
			}
			int x2 = static_cast<int>(x1 + 0.5);
			output.at<uchar>(i, j) = x2;
		}
	}
}
void linear(Mat& output, int x1, int y1, int x2, int y2) {
	int row = output.rows;
	int col = output.cols;
	for (int i =0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int old = output.at<uchar>(i, j);
			double xx1;
			if (old >= x1 && old <= x2) {
				xx1 = ((y2 - y1) / (x2 - x1))*(old - x1) + y1;
			}
			else if (old < x1) {
				 xx1 = (y1 / x1)*old;
			}
			else {
				xx1 = ((255 - y2) / (255 - x2))*(old - x2) + y2;
			}

			if (xx1 > 255) {
				xx1 = 255;
			}
			else if (xx1 < 0) {
				xx1 = 0;
			}
			int xx2 = static_cast<int>(xx1 + 0.5);
			output.at<uchar>(i, j) = xx2;
		}
	}
}
void histogram(Mat& output) {
	int row = output.rows;
	int col = output.cols;
	int total = row * col;
	int memory[256];
	for (int i = 0; i < 256; i++) {
		memory[i] = 0;
	}
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int data = output.at<uchar>(i, j);
			memory[data] += 1;
		}
	}
	int sum = 0;
	for (int i = 0; i < 256; i++) {
		sum += memory[i];
		memory[i] = sum;
	}
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int nnew = 255 *memory[output.at<uchar>(i, j)] / total;
			output.at<uchar>(i, j) = nnew;
		}
	}
}
void average(Mat& img) {
	int col = img.cols;
	int row = img.rows;
	Mat kernel(3, 3, CV_32F,Scalar(0));
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			kernel.at<float>(i, j) =(float)1/9;
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
    kernel_y.at<float>(2,1) = 1;
	Mat img2;
	filter2D(img, img2, img.depth(), kernel_y);
	img = img1 + img2;
	/*for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);
		}
	}*/
}
void sobel(Mat& img) {
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
	img = img1 + img2;
	/*for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);
		}
	}*/
}
int main() {
	Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.1.png", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("origin",img);
	Mat img1=img.clone();
	log(img1,44);
	imshow("log", img1);
	Mat img2=img.clone();
	index(img2, 1,3.6);
	imshow("index", img2);
	Mat img3 = img.clone();
	linear(img3, 100, 10, 200, 180);
	imshow("linear", img3);
	Mat img4 = img.clone();
	histogram(img4);
	imshow("histogram equalization", img4);
	Mat img5 = img.clone();
	average(img5);
	imshow("average", img5);
	Mat img6 = img.clone();
	robert(img6);
	imshow("robert", img6);
	imshow("sharpen robert", img + img6);
	Mat img7 = img.clone();
	sobel(img7);
	imshow("sobel", img7);
	imshow("sharpen sobel", img + img7);
	waitKey(0);
	return 0;
}