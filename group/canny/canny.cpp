#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<math.h>
#include<vector>
using namespace cv;
int vec1[1000][1000];
int table[1000][1000];
void gaos(Mat& in) {
	Mat gao(3, 3, CV_32F, Scalar(0));
	gao.at<float>(0, 0) = (float)1 / 16;
	gao.at<float>(0,1) = (float)1 / 8;
	gao.at<float>(0,2) = (float)1 / 16;
	gao.at<float>(1, 0) = (float)1 / 8;
	gao.at<float>(1, 1) = (float)1 /4;
	gao.at<float>(1, 2) = (float)1 / 8;
	gao.at<float>(2, 0) = (float)1 / 16;
	gao.at<float>(2,1) = (float)1 / 8;
	gao.at<float>(2, 2) = (float)1 / 16;
	filter2D(in,in, in.depth(), gao);
}
void sobel(Mat& img,int(*vec1)[1000]) {
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
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			double here = atan2((int)img2.at<uchar>(i, j), (int)img1.at<uchar>(i, j));
			if (here >= 0 && here <= 3.14 / 8) {
				vec1[i][j]= 1;
			}
			else if (here > 3.14 / 8 && here <= 3.14 * 3 / 8) {
				vec1[i][j] = 2;
			}
			else if (here > 3.14 * 3 / 8 && here <= 3.14 * 5 / 8) {
				vec1[i][j] = 3;
			}
			else {
				vec1[i][j] = 4;
			}
		}
	}
	img = img1 + img2;
}
void devide(Mat in,Mat& imgh,Mat& imgl, int high, int low) {
	for (int i = 0; i < in.rows; i++) {
		for (int j = 0; j < in.cols; j++) {
			if (in.at<uchar>(i, j) >= high) {
				imgh.at<uchar>(i, j) = in.at<uchar>(i, j);
			}
			if (in.at<uchar>(i, j) >= low) {
				imgl.at<uchar>(i, j) = in.at<uchar>(i, j);
			}
			imgl.at<uchar>(i, j) = imgl.at<uchar>(i, j) - imgh.at<uchar>(i, j);
		}
	}
}

void connect(Mat& high, Mat& low) {
	int col = high.cols;
	int row = high.rows;
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 1000; j++) {
			table[i][j] = 0;
		}
	}

	bool find = false;
f1:
	find = false;
	for (int i = 1; i < row - 1; i++) {
			for (int j = 1; j < col - 1; j++) {
				if (low.at<uchar>(i, j) == 1 || high.at<uchar>(i,j)!=1 || table[i][j]==1) {
				//	std::cout << i << "," << j << " ===  ";
					continue;
				}
				if (high.at<uchar>(i -1, j-1) != 1) {
					table[i][j] = 1;
				}
				else if (high.at<uchar>(i - 1, j) != 1) {
					table[i][j] = 1;
				}
				else if (high.at<uchar>(i - 1, j + 1) != 1) {
					table[i][j] = 1;
				}
				else if (high.at<uchar>(i , j - 1) != 1) {
					table[i][j] = 1;
				}
				else if (high.at<uchar>(i, j + 1) != 1) {
					table[i][j] = 1;
				}
				else if (high.at<uchar>(i+1, j - 1) != 1) {
					table[i][j] = 1;
				}
				else if (high.at<uchar>(i + 1, j ) != 1) {
					table[i][j] = 1;
				}
				else if (high.at<uchar>(i + 1, j+1) != 1) {
					table[i][j] = 1;
				}

				if (table[i][j] == 1) {
					high.at<uchar>(i, j) = low.at<uchar>(i, j);
					find = true;
				//	std::cout << i << "," << j<<std:: endl;
				}
			}
		}
	if (find == true)
		goto f1;
	/*for (int i = 0; i < row; i++) {
		for (int j = 0; j < col ; j++) {
			if (table[i][j] == 1) {
				high.at<uchar>(i, j) = low.at<uchar>(i, j);
			}
		//	std::cout << table[i][j] << std::endl;
		}
	}*/
}

int main() {
	//Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\group\\canny\\111.png", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("origin", img);
	Mat img1 = img.clone();
	gaos(img1);
	imshow("gaos", img1);
	int col = img1.cols;
	int row = img1.rows;
	/*int** memory;
	memory = (int**)malloc(sizeof(int*)*col);
	for (int i = 0; i < col; i++)
		memory[i] = (int*)malloc(sizeof(int)*row);   //memory[col][row]*/
   //��ʼ��һ��3x3�ľ�����ֵΪ 333
	sobel(img1,vec1); //ʹ��sobel���Ӽ����ݶȷ�ֵ
	imshow("sobel", img1);
	std::cout <<row << "," << col << std::endl;
	//int a=img1.at<uchar>(467, 33);
	Mat img3 = img1.clone();
	for (int i =1; i < row-1; i++) {
		for (int j = 1; j < col-1; j++) {
			if (vec1[i][j] == 3) {
				if (img1.at<uchar>(i, j) < img1.at<uchar>(i, j - 1)|| img1.at<uchar>(i, j) < img1.at<uchar>(i, j + 1)) {
					img3.at<uchar>(i, j) = 0;
				}
			}
		else if (vec1[i][j] ==4) {
				if (img1.at<uchar>(i, j) < img1.at<uchar>(i- 1, j +1) || img1.at<uchar>(i, j) < img1.at<uchar>(i + 1, j -1)) {
					img3.at<uchar>(i, j) = 0;
				}
			}
		else if (vec1[i][j] == 1) {
					if (img1.at<uchar>(i, j) < img1.at<uchar>(i - 1, j)|| img1.at<uchar>(i, j) < img1.at<uchar>(i+1,j)) {
						img3.at<uchar>(i, j) = 0;
					}
			}
			else if (vec1[i][j] == 2) {
				if (img1.at<uchar>(i, j) < img1.at<uchar>(i + 1, j +1) || img1.at<uchar>(i, j) < img1.at<uchar>(i - 1, j -1)) {
					img3.at<uchar>(i, j) = 0;
				}
			}
		}
	}
	/*for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			img1.at<uchar>(i, j) = 255 - img1.at<uchar>(i, j);
		}
	}*/
	imshow("after", img3);
	Mat imgh(row, col, CV_8U,1);
	Mat imgl(row, col, CV_8U,1);
	//Mat imgh = img1.clone();
	//Mat imgl = img1.clone();
	devide(img3,imgh,imgl, 70, 200);
	imshow("strong", imgh);
	imshow("weak", imgl);
	Mat strong = imgh.clone();
	connect(imgh, imgl);
	imshow("final", imgh);
	Mat gap = imgh - strong;
	imshow("gap", gap);

	Mat img11 =img.clone();
	 Canny(img,img11, 70, 200);
	/* Mat img22 = img11.clone();
	 for (int i = 0; i < img11.rows; i++) {
		 for (int j = 0; j < img11.cols; j++) {
			 img22.at<uchar>(i, j) = 255 - img22.at<uchar>(i, j);
		 }
	 }*/
	imshow("out",img11);
	waitKey(0);
	return 0;
}