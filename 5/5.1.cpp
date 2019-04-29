#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<algorithm>
#include <stack>
#include<math.h>
#include<vector>
using namespace cv;
using namespace std;
int vec1[1000][1000];
int table[1000][1000];
static Point connects[8] = { Point(-1, -1), Point(0, -1), Point(1, -1), Point(1, 0), Point(1, 1), Point(0, 1), Point(-1, 1), Point(-1, 0) }; //����8����
void gaos(Mat& in) {  //��˹ģ����canny�㷨�ã�
	Mat gao(3, 3, CV_32F, Scalar(0));
	gao.at<float>(0, 0) = (float)1 / 16;
	gao.at<float>(0, 1) = (float)1 / 8;
	gao.at<float>(0, 2) = (float)1 / 16;
	gao.at<float>(1, 0) = (float)1 / 8;
	gao.at<float>(1, 1) = (float)1 / 4;
	gao.at<float>(1, 2) = (float)1 / 8;
	gao.at<float>(2, 0) = (float)1 / 16;
	gao.at<float>(2, 1) = (float)1 / 8;
	gao.at<float>(2, 2) = (float)1 / 16;
	filter2D(in, in, in.depth(), gao);
}
void sobel(Mat& img, int(*vec1)[1000]) {  //sobel��ȡ��Ե��canny�㷨��),vec1������¼ÿ�����Ӧ�ݶȷ��ֵİ�������
	int col = img.cols;
	int row = img.rows;
	Mat kernel_y(3, 3, CV_32F, Scalar(0));
	kernel_y.at<float>(0, 0) = -1;
	kernel_y.at<float>(0, 1) = -2;
	kernel_y.at<float>(0, 2) = -1;
	kernel_y.at<float>(2, 0) = 1;
	kernel_y.at<float>(2, 1) = 2;
	kernel_y.at<float>(2, 2) = 1;
	Mat img1;
	filter2D(img, img1, img.depth(), kernel_y);
	Mat kernel_x(3, 3, CV_32F, Scalar(0));
	kernel_x.at<float>(0, 0) = -1;
	kernel_x.at<float>(0, 2) = 1;
	kernel_x.at<float>(1, 0) = -2;
	kernel_x.at<float>(1, 2) = 2;
	kernel_x.at<float>(2, 0) = -1;
	kernel_x.at<float>(2, 2) = 1;
	Mat img2;
	filter2D(img, img2, img.depth(), kernel_x);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			double here = atan2((int)img1.at<uchar>(i, j), (int)img2.at<uchar>(i, j));
			if (here < 0) {
				here = -here;
			}
			if ((here >= 0 && here <= 3.14/8)||(here>3.14*7/8&&here<=3.14)) {
				vec1[i][j] = 1;
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
void devide(Mat in, Mat& imgh, Mat& imgl, int high, int low) { //��in���ոߵ���ֵ�ֳ�����ͼ
	for (int i = 0; i < in.rows; i++) {
		for (int j = 0; j < in.cols; j++) {
			if (in.at<uchar>(i, j) >= high) {
				imgh.at<uchar>(i, j) = in.at<uchar>(i, j);
			}
			if (in.at<uchar>(i, j) >= low&&imgh.at<uchar>(i,j)==0) {
				imgl.at<uchar>(i, j) = in.at<uchar>(i, j);
			}
		//	imgl.at<uchar>(i, j) = imgl.at<uchar>(i, j) - imgh.at<uchar>(i, j);
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
			if (low.at<uchar>(i, j) ==0||table[i][j]==1) {
				//std::cout << i << "," << j << " ===  ";
				continue;
			}
			if (high.at<uchar>(i - 1, j - 1) !=0) {
				table[i][j] = 1;
			}
			else if (high.at<uchar>(i - 1, j) !=0) {
				table[i][j] = 1;
			}
			else if (high.at<uchar>(i - 1, j + 1) != 0) {
				table[i][j] = 1;
			}
			else if (high.at<uchar>(i, j - 1) != 0) {
				table[i][j] = 1;
			}
			else if (high.at<uchar>(i, j + 1) !=0) {
				table[i][j] = 1;
			}
			else if (high.at<uchar>(i + 1, j - 1) != 0) {
				table[i][j] = 1;
			}
			else if (high.at<uchar>(i + 1, j) !=0) {
				table[i][j] = 1;
			}
			else if (high.at<uchar>(i + 1, j + 1) != 0) {
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

Mat threshold(Mat img, int turn) {  //��ֵ�ָ�
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

 Mat onlystrong(Mat img1) {
	 Mat img3 = img1.clone();
	 int row = img1.rows;
	 int col = img1.cols;
	for (int i = 1; i < row - 1; i++) {   //��Բ�ͬ���ݶȷ��ַ�����з��������
		for (int j = 1; j < col - 1; j++) {
			if (vec1[i][j] == 3) {
				if (img1.at<uchar>(i, j) < img1.at<uchar>(i, j - 1) || img1.at<uchar>(i, j) < img1.at<uchar>(i, j + 1)) {
					img3.at<uchar>(i, j) = 0;
				}
			}
			else if (vec1[i][j] == 4) {
				if (img1.at<uchar>(i, j) < img1.at<uchar>(i - 1, j + 1) || img1.at<uchar>(i, j) < img1.at<uchar>(i + 1, j - 1)) {
					img3.at<uchar>(i, j) = 0;
				}
			}
			else if (vec1[i][j] == 1) {
				if (img1.at<uchar>(i, j) < img1.at<uchar>(i - 1, j) || img1.at<uchar>(i, j) < img1.at<uchar>(i + 1, j)) {
					img3.at<uchar>(i, j) = 0;
				}
			}
			else {
				if (img1.at<uchar>(i, j) < img1.at<uchar>(i + 1, j + 1) || img1.at<uchar>(i, j) < img1.at<uchar>(i - 1, j - 1)) {
					img3.at<uchar>(i, j) = 0;
				}
			}
		}
	}
	return img3;
}
Mat Kenny(Mat in) {  //Canny�㷨����
	Mat img1 = in.clone();
	gaos(img1);  //�ȸ�˹ģ��
	int col =img1.cols;  //��¼ԭͼ��ĳ��Ϳ�
	int row = img1.rows;
	sobel(img1, vec1);  //sobel��ȡͼ�����
	Mat img3=onlystrong(img1); //�Ǽ���ֵ����
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (img3.at<uchar>(i, j) == 0) {
				img3.at<uchar>(i, j) = 1;
			}
		}
	}
	Mat imgh(row, col, CV_8U,Scalar(0));  //�����ߵ���ֵ����������ͼ��
	Mat imgl(row, col, CV_8U,Scalar(0));
	devide(img3, imgh, imgl, 200,70);//��70��200�����ߵ���ֵ
	connect(imgh, imgl); //���ӽϳ��ı�Ե
	return imgh;
	/*Mat img11 =in.clone();
	Canny(in, img11, 70, 200);
	return img11;*/
}
Mat RegionGrowing(Mat in,int x,int y,int limit)   //���ӵ�����x,y����ֵΪlimit�������������㷨
{
	if (x > in.rows || y > in.cols) {
		return in;
	}
	Mat out(in.rows, in.cols, CV_8U, Scalar(0));  //����ͼ��ĳ�ʼ�㶼��0

	//�����ӵ���ջ 
	stack<Point> pointStack;
	Point seedpoint(x, y);
	pointStack.push(seedpoint);

	//��OutputData�б��������Ϊ��ɫ
	out.at<uchar>(x,y) = 255;
	int initialColor = in.at<uchar>(x,y);
	Point p;
	int  growLable = 0;//����Ƿ�������
	int  inputValue = 0;//��������ĻҶ�ֵ
	while (!pointStack.empty())
	{
		//ȡ�� ���ӵ�
		Point  topPoint = pointStack.top();
		//��stack��ɾ���õ�
		pointStack.pop();
		//���������صİ�����
		for (int i = 0; i < 8; i++)
		{
			p = topPoint +connects[i];//p�Ǵ�������
			int x = p.x;
			int y = p.y;
			   //ͼ��ı߽���
			if (x<0 || y<0 || y>(in.cols - 1) || x>(in.rows - 1))
			{
				continue;
			}
			growLable = out.at<uchar>(x,y);  
			inputValue = in.at<uchar>(x,y);
			//�ε�δ�����Ҵ����������һ�����ӵ�ĻҶȲ�ľ���ֵС�ڸ���ֵ������
			if (growLable == 0 && abs(inputValue - initialColor) < limit)
			{
				out.at<uchar>(x,y) = 180;//�û�ɫ��ʾ����
			   //���˵���ջ
				pointStack.push(p);
			}
		}

	}
	return out;
}

int main() {
	Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("orign", img);
	cout << img.rows << " " << img.cols << endl;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img.at<uchar>(i, j) == 0) {
				img.at<uchar>(i, j) = 1;
			}
		}
	}
	Mat img1 = threshold(img, 180);
	imshow("threshold", img1);
	Mat img2 = RegionGrowing(img, 350, 550,40);
	imshow("regiongrowing", img2);
	Mat img3 = Kenny(img);
	imshow("Cannyme", img3);
	Mat img11 = img.clone();
	Canny(img, img11, 70, 200);
	imshow("Canny", img11);
	waitKey(0);
	return 0;
}