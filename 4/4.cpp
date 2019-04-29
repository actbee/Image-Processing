#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<algorithm>
using namespace cv;

#define PixcelMax 255
#define PixcelMin 0
int mem[100000];

double Gaussiannoise(double mu, double sigma) {
	//参考Marsaglia和Bray的方法生成高斯分布随机数序列,mu均值sigma方差
	static double V1, V2, S;
	static int phase = 0;
	double X;
	double U1, U2;
	if (phase == 0) {
		do {
			U1 = (double)rand() / RAND_MAX;   
			U2 = (double)rand() / RAND_MAX;

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	}
	else {
		X = V2 * sqrt(-2 * log(S) / S);
	}
	phase = 1 - phase;
	return mu + sigma * X;
}

void addgaussiannoise(Mat& img, double mu, double sigma, int k) {
	for (int x = 0; x < img.rows; x++) {
		for (int y = 0; y < img.cols; y++) {
			double temp = img.at<uchar>(x, y)
				+ k *Gaussiannoise(mu, sigma);  //对每个像素点操作加高斯噪声
			if (temp > PixcelMax)   //对超出边界的值进行约束
				temp = PixcelMax;
			else if (temp < PixcelMin)
				temp = PixcelMin;
			img.at<uchar>(x, y) = temp;
		}
	}
}
void addsaltnoise(Mat& img, double SNR) {  //添加椒盐噪声，SNR表示(洁净图片中的像素点的灰度值之和）/abs（噪声图片的灰度值之和-洁净图片中的灰度值之和）
	int SP = img.rows*img.cols;  //计算总的像素数目
	int NP = SP * (1 - SNR);  //计算要加噪的像素数目
	for (int i = 0; i < NP; i++) {   //随机获取加噪像素位置
		int x = rand()%img.rows;
		int y =rand()%img.cols;
		int r = rand() % 2;
		if (r) {    //随机指定其像素值为0或者255
			img.at<uchar>(x, y) = 0;
		}
		else {
			img.at<uchar>(x, y) = 255;
		}
	}
}
Mat  median(Mat img)
{
	Mat out = img.clone();
	for (int i =1; i < img.rows-1; i++) {
		for (int j = 1; j < img.cols-1; j++) {
			mem[0] = img.at<uchar>(i - 1, j - 1);
			mem[1] = img.at<uchar>(i - 1, j );
			mem[2] = img.at<uchar>(i - 1, j + 1);
			mem[3] = img.at<uchar>(i , j - 1);
			mem[4] = img.at<uchar>(i , j );
			mem[5] = img.at<uchar>(i , j + 1);
			mem[6] = img.at<uchar>(i + 1, j - 1);
			mem[7] = img.at<uchar>(i +1, j);
			mem[8] = img.at<uchar>(i +1, j+1);
			std::sort(mem, mem + 9);
			int mid = mem[4];
			out.at < uchar >(i,j)= mid;
		}
	}
	return out;
}
int fit(Mat img,int size,int i,int j) { //传入的size必须是奇数
	if (size % 2 == 0) {
		return -1;
	}
	Mat out = img.clone();
			int num = 0;
			for (int ii = i - size / 2; ii < i + size / 2; ii++) {
				for (int jj = j - size / 2; jj < j + size / 2; jj++) {
					mem[num] = img.at<uchar>(ii, jj);
					num++;
				}
			}
			std::sort(mem, mem +num);  //sort后结果从小到大
			int mid = mem[num/2];
			int low = mem[0];
			int high = mem[num-1];
			if (mid - low > 0 && mid - high < 0) {
				if (img.at<uchar>(i, j) - low > 0 && img.at<uchar>(i, j) - high < 0) {
					return img.at<uchar>(i, j);
				}
				else {
					return mid;
				}
			}
			size+=2;
			if ( i - size /2<0|| j - size / 2<0|| i + size / 2>img.rows|| j + size / 2>img.cols) {
				return mid;
			}
			fit(img, size, i, j);
}
Mat fitmedian(Mat img) {
	Mat out = img.clone();
	for (int i = 1; i < img.rows-1; i++) {
		for (int j = 1; j < img.cols-1; j++) {
			int output=fit(img, 3, i, j);
			out.at<uchar>(i, j) = output;
		}
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

int main() {
	//Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.1.png", CV_LOAD_IMAGE_GRAYSCALE);
	std::cout << img.cols << " " << img.rows << std::endl;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img.at<uchar>(i, j) ==0) {
				img.at<uchar>(i, j) = 1;
			}
			else if (img.at<uchar>(i, j) == 255) {
				img.at<uchar>(i, j) =253;
			}
		}
	}
	imshow("origin", img);
	Mat img1 = img.clone();
	addgaussiannoise(img1, 0, 1,16);
	imshow("gaussian", img1);
	Mat img2 = img.clone();
	addsaltnoise(img2, 0.8);
	imshow("salt", img2);
	Mat img11 = img1.clone();
	average(img11);
	imshow("average_gaussian", img11);
	Mat img21 = img2.clone();
	average(img21);
	imshow("average_salt", img21);
	Mat img12 = median(img1);
	imshow("median_gaussian", img12);
	Mat img22 = median(img2);
	imshow("median_salt", img22);
	Mat img13 = fitmedian(img1);
	imshow("fitmedian_gaussian", img13);
	Mat img23 = fitmedian(img2);
	imshow("fitmedian_salt", img23);
	waitKey(0);
	return 0;
}