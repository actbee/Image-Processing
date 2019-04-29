#include <iostream>  
#include <opencv2/opencv.hpp>  


using namespace cv;

void Resize(Mat img) {
	float scalew = 0.5;
	float scaleh = scalew;
	Mat src = img;
	Mat dst;
	if (src.empty()) {
		return;
	}
	int width = static_cast<float>(src.cols*scalew);
	int height = static_cast<float>(src.rows*scaleh);
	resize(src, dst, Size(width, height));
	imshow("src", src);
	imshow("dst", dst);
	waitKey();
}

int main()
{
	// ����һ��ͼƬ��poyanghu��Сͼ��    
	//Mat img = imread("F:\\screen\\Cities 2018-09-22 23-48-33-53.bmp");
	Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\1\\822.jpg");
	// ����һ����Ϊ "ͼƬ"����    
	Resize(img);
	waitKey(500000);
}