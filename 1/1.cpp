#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace cv;

Mat rresize(Mat img, float rate) {
	float scalew = rate;
	float scaleh = scalew;
	Mat src = img;
	Mat dst;
	if (src.empty()) {
		return img;
	}
	int width = static_cast<float>(src.cols*scalew);
	int height = static_cast<float>(src.rows*scaleh);
	resize(src, dst, Size(width, height));
	return dst;
}


Mat move(Mat const& src, int dx, int dy)
{
	const int rows = src.rows;
	const int cols = src.cols;
	Mat resultImg(src.size(), src.type());
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			int x = j - dx;
			int y = i - dy;
			if (x >= 0 && y >= 0 && x < cols&&y < rows) {         //here ptr is a pointer.ptr<>(0)points at the first one of first line,and ptr<>(0)[1]points at the second line of first line
				resultImg.at<Vec3b>(i, j) = src.ptr<Vec3b>(y)[x];
			}
		}
	}
	return resultImg;
}

Mat rotate(Mat img, float angle) {                      
	Mat resultImg(img.size(), img.type());
	Point2f center = Point2f(static_cast<float>(img.cols / 2), static_cast<float>(img.rows / 2));  //to redefine the center point to rotate
	Mat rot = getRotationMatrix2D(center, angle, 1);               //(a,b,c) a refers to the center point to rotate,b refers to the anger and c is the scale rate
	Rect bbox = RotatedRect(center,img.size(), angle).boundingRect(); //RotatedRect returns a rotated matrix and boundingRect returns a smallest rectangle contains all the img
	rot.at<double>(0, 2) += bbox.width / 2.0 - center.x; // use these 2 lines to make sure the img's center is exactly at the center of the new window
	rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;
	Mat dst;
	warpAffine(img, dst, rot, bbox.size());//rot is the matrix to change and bbox.size() is the output size
	return dst;
}

Mat mirro(Mat img, int num) {
	switch (num) {
	case 0: { //0 means mirro by Y
		int row = img.rows;
		int col = img.cols;
		Mat ret = img.clone();
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				ret.at<Vec3b>(i, j) = img.at<Vec3b>(i, col - 1 - j);
			}
		}
		return ret;
	}
	case 1: { //1 means mirro by X
		int row = img.rows;
		int col = img.cols;
		Mat ret = img.clone();
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				ret.at<Vec3b>(i, j) = img.at<Vec3b>(row-1-i,j);
			}
		}
		return ret;
	}
	default: {
		return img;
	}
	}
}

/*Mat nearest(Mat img, double  a,double b) {
	int row = cvRound(img.rows*a);
	int col = cvRound(img.cols*b); //use cvRound to round a float
	Mat res(row, col, img.type());
	int i, j, x, y;
	for (i = 0; i < row; i++) {
		x = static_cast<int>((i + 1) /a + 0.5) - 1;
		for (j = 0; j < col; j++) {
			y = static_cast<int>((j + 1) / b + 0.5) - 1;
			res.at<Vec3b>(i,j) = img.at<Vec3b>(x, y);
		}
	}
	return res;
}*/
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
int main()
{   
	//Mat img = imread("F:\\screen\\Cities 2018-09-22 23-48-33-53.bmp");
	Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\1\\822.jpg");
	//("origin", img);
	img=rresize(img,0.3);
	imshow("resize", img);
	Mat img2 = move(img, 100, 100);
	imshow("move", img2);
	Mat img3= rotate(img, 65);
	imshow("rotate", img3);
	Mat img4 = mirro(img, 1);
    	imshow("mirro", img4);
	//Mat img4 = nearest(img, 0.6,1.2);
    Mat img5;
	resize(img, img5, Size(600,600),0,0,INTER_NEAREST);//input,output,new size,zoom x rate,zoom y rate,the way to resize
	imshow("最近邻插值", img5);
	Mat img6;
	resize(img, img6, Size(600, 600), 0, 0, INTER_LINEAR);
	imshow("双线性插值", img6);
	Mat img7;
	resize(img, img7, Size(600,600), 0, 0, INTER_CUBIC);
	imshow("立方插值", img7);
	Mat img8;
	wave(img, img8);
	imshow("wave", img8);
	waitKey();
	return 0;
}