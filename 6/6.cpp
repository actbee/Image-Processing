#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
using namespace cv;
Mat bl(Mat img,int turn) {
	Mat out = img.clone();
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if(img.at<uchar>(i,j)>turn){
				out.at<uchar>(i, j) = 255;
			}
			else {
				out.at<uchar>(i, j) = 0;
			}
		}
	}
	return out;
}
Mat corrosion(Mat in, Mat use, int a, int b) {  //对于传入的参数，in是处理的图像，use是自定义的结构元（这里默认是矩形样式),a,b是结构元修改位的坐标（从0开始）
	Mat out = in.clone();
	int row = use.rows;
	int col = use.cols;
	for (int i = 0; row - 1 + i < in.rows; i++) {
		for (int j = 0; col - 1 + j < in.cols; j++) {

			for (int ii = 0; ii < row; ii++) {
				for (int jj = 0; jj < col; jj++) {
					if (in.at<uchar>(i+ii, j+jj) == 255) {
						out.at<uchar>(i + a, j + b) = 255;
						goto ne;
					}
				}
			}
		ne:
			continue;
		}
	}
	return out;
}
Mat expand(Mat in, Mat use, int a, int b) {
	Mat out = in.clone();
	int row = use.rows;
	int col = use.cols;
	for (int i = 0; row - 1 + i < in.rows; i++) {
		for (int j = 0; col - 1 + j < in.cols; j++) {

			for (int ii = 0; ii < row; ii++) {
				for (int jj = 0; jj < col; jj++) {
					if (in.at<uchar>(ii+i, jj+j) == 0) {
						out.at<uchar>(i + a, j + b) = 0;
						goto ne;
					}
				}
			}
		ne:
			continue;
		}
	}
	return out;
}
Mat open(Mat in, Mat use, int a, int b) {
	Mat mid=corrosion(in, use, a, b);
	Mat out = expand(mid, use, a, b);
	return out;
}
Mat close(Mat in, Mat use, int a, int b) {
	Mat mid =expand(in, use, a, b);
	Mat out = corrosion(mid, use, a, b);
	return out;
}
int main() {
	Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.1.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat binary=bl(img,195);
	imshow("origin", img);
	imshow("binary", binary);
	Mat d1(3, 3, CV_8U,Scalar(0));
	Mat d2(3, 5, CV_8U, Scalar(0));
	Mat c1=corrosion(binary,d1, 1, 1);
	imshow("corrosion",c1);
	Mat e1 = expand(binary, d1, 1, 1);
	imshow("expand", e1);
	Mat o1 = open(binary, d1, 1, 1);
	imshow("open1", o1);
	Mat clo1 = close(binary, d1, 1, 1);
	imshow("close1", clo1);
	Mat c2 = corrosion(binary, d2, 2, 3);
	imshow("corrosion2", c2);
	Mat e2 = expand(binary, d2, 2, 3);
	imshow("expand2", e2);
	Mat o2 = open(binary, d2, 2,3);
	imshow("open2", o2);
	Mat clo2 = close(binary, d2,2, 3);
	imshow("close2", clo2);
	waitKey();
	return 0;
}