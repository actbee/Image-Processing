#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<vector>
using namespace cv;

class Histogram {
private:
	int histsize[1];
	float hranges[2];
	const float* ranges[1];
	int channels[1];
public:
	Histogram() {
		histsize[0] = 256;
		hranges[0] = 0.0;
		hranges[1] = 256.0;
		ranges[0] = hranges;
		channels[0] = 0;
	}
	Mat gethistogram(const Mat& img) {
		Mat hist;
		calcHist(&img, 1, channels, Mat(), hist, 1, histsize, ranges);
		return hist;
	}
	Mat gethistogramimg(const Mat& img, int zoom = 1) {
		Mat hist = gethistogram(img);
		return getimgofhistogram(hist, zoom);
	}
	static Mat getimgofhistogram(const Mat& hist, int zoom) {
		double maxVal = 0;
		double minVal = 0;
		minMaxLoc(hist, &minVal, &maxVal, 0, 0);
		int histSize = hist.rows;
		Mat histImg(histSize*zoom, histSize*zoom, CV_8U, Scalar(255));
		int hpt = static_cast<int>(0.9*histSize);
		for (int h = 0; h < histSize; h++) {
			float binVal = hist.at<float>(h);
			if (binVal > 0) {
				int intensity = static_cast<int>(binVal*hpt / maxVal);
				line(histImg, Point(h*zoom, histSize*zoom), Point(h*zoom, (histSize - intensity)*zoom), Scalar(0), zoom);
			}
		}
		return histImg;
	}
};

void salt(Mat img, int n) {
	int i, j;
	for (int k = 0; k < n; k++) {
		i = std::rand() % img.cols;
		j = std::rand() % img.rows;
		if (img.type() == CV_8UC1) {
			img.at<uchar>(j, i) = 255;
		}
		else if (img.type() == CV_8UC3) {
			img.at<Vec3b>(j, i)[0] = 255;
			img.at<Vec3b>(j, i)[1] = 255;
			img.at<Vec3b>(j, i)[2] = 255;
		}
	}
}

/*void sharpen2D(const Mat& img, Mat& result) {
	Mat kernel(3, 3, CV_32FC3, Scalar(0, 0, 0));
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			std::cout << kernel.at<Vec3f>(i, j)[0] << " " << kernel.at<Vec3f>(i, j)[1] << " " << kernel.at<Vec3f>(i, j)[2] << std::endl;
		}
	}
	kernel.at<Vec3f>(0, 1)[0] = 1.0;
	kernel.at<Vec3f>(1, 0)[0] = 1.0;
	kernel.at<Vec3f>(1, 1) [0]= -4.0;
	kernel.at<Vec3f>(1, 2)[0] = 1.0;
	kernel.at<Vec3f>(2, 1)[0] = 1.0;
	kernel.at<Vec3f>(0, 1)[1] = 1.0;
	kernel.at<Vec3f>(1, 0)[1] = 1.0;
	kernel.at<Vec3f>(1, 1)[1] = -4.0;
	kernel.at<Vec3f>(1, 2)[1] = 1.0;
	kernel.at<Vec3f>(2, 1)[1] = 1.0;
	kernel.at<Vec3f>(0, 1)[2] = 1.0;
	kernel.at<Vec3f>(1, 0)[2] = 1.0;
	kernel.at<Vec3f>(1, 1)[2] = -4.0;
	kernel.at<Vec3f>(1, 2)[2] = 1.0;
	kernel.at<Vec3f>(2, 1)[2] = 1.0;
    filter2D(img, result, img.depth(), kernel);
}*/
/*void average2D(const Mat& img, Mat& result) {

	Mat kernel(3, 3, CV_32FC3, Scalar(0,0,0));
	imshow("kernel", kernel);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			kernel.at<Vec3f>(i, j)[0] = (float)1 / 9;
			kernel.at<Vec3f>(i, j)[1] = (float)1 / 9;
			kernel.at<Vec3f>(i, j)[2] = (float)1 / 9;
		}
	}
		filter2D(img, result, img.depth(), kernel);
}*/
void sharpen2D(const Mat& img, Mat& result) {
	Mat kernel(3, 3, CV_32F, Scalar(0));
	kernel.at<float>(0, 1) = 1.0;
	kernel.at<float>(1, 0) = 1.0;
	kernel.at<float>(1, 1) = -4.0;
	kernel.at<float>(1, 2) = 1.0;
	kernel.at<float>(2, 1) = 1.0;
	filter2D(img, result, img.depth(), kernel);
	result = img + result;
}
void average2D(const Mat& img, Mat& result) {
	Mat kernel(3, 3, CV_32F, Scalar(0));
	imshow("kernel", kernel);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			kernel.at<float>(i, j) = (float)1 / 9;
		}
	}
	filter2D(img, result, img.depth(), kernel);
}
void wave(const Mat& img, Mat &result) {
	Mat srcX(img.rows, img.cols, CV_32F);
	Mat srcY(img.rows, img.cols, CV_32F);
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			srcX.at<float>(i, j) = j;
			srcY.at<float>(i, j) = i + 5 * sin(j / 10.0);
		}
	}
	remap(img, result, srcX, srcY, INTER_LINEAR);
}
int main() {
	//Mat img1 = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.jpg",CV_LOAD_IMAGE_COLOR);
	//circle(img2, Point(150, 150), 65, 0, 3);
	salt(img2, 3000);
	imshow("origin", img2);
	//imshow("1", img1);
	//imshow("2", img2);
	Mat img3;
	Mat img4;
	average2D(img2, img3);
	sharpen2D(img3, img4);
	imshow("a1", img3);
	imshow("as", img4);
	Mat img5;
	Mat img6;
	sharpen2D(img2, img5);
	average2D(img5, img6);
	imshow("s1", img5);
	imshow("sa", img4);
	//Mat img5;
	//wave(img1, img5);
//	imshow("5", img5);
	//Histogram h;
	//Mat img314a = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\14a.png", CV_LOAD_IMAGE_GRAYSCALE);
/*	Mat result=h.gethistogram(img314a);
	for (int i = 0; i < 255; i++) {
		std::cout << "Value " <<i << "=" << result.at<float>(i) << std::endl;
	}*/
	/*average2D(img314a, img314a);
	imshow("3.14a", img314a);
	imshow("Histogram",h.gethistogramimg(img314a));
	Mat img314b = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\14b.png", CV_LOAD_IMAGE_GRAYSCALE);
	average2D(img314b, img314b);
	imshow("3.14b", img314b);
	imshow("Histogram2", h.gethistogramimg(img314b));*/
	waitKey(0);
	return 0;
}