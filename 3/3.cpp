#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
using namespace cv;
Mat cut(int row, int col, Mat img) {
	Mat out(row, col, CV_8U, 1);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			out.at<uchar>(i, j) = img.at<uchar>(i, j);
		}
	}
	return out;
}
Mat ideal_Low_Pass_Filter(Mat in,double D0) {
	Mat img;
	img = in.clone();
	//调整图像加速傅里叶变换
	int M = getOptimalDFTSize(img.rows);
	int N = getOptimalDFTSize(img.cols);
	Mat padded;
	copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0));
	//记录傅里叶变换的实部和虚部
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexImg;
	merge(planes, 2, complexImg);
	//进行傅里叶变换
	dft(complexImg, complexImg);
	//获取图像
	Mat mag = complexImg;
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	//修剪频谱，如果图像的行或者列是奇数的话，那其频谱是不对称的，因此要修剪
	//这里为什么要用  &-2这个操作，我会在代码后面的 注2 说明

	//获取中心点坐标
	int cx = mag.cols / 2;
	int cy = mag.rows / 2;
	//调整频域
	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));

	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	//处理按公式保留中心部分
	for (int y = 0; y < mag.rows; y++) {
		double* data = mag.ptr<double>(y);
		for (int x = 0; x < mag.cols; x++) {
			double d = sqrt(pow((y - cy), 2) + pow((x - cx), 2));
			double h = 1;
			if (d > D0) {
				h = 0;
			}
			data[x] *= h;
		}
	}
	//再调整频域
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	//逆变换
	Mat invDFT, invDFTcvt;
	idft(mag, invDFT, DFT_SCALE | DFT_REAL_OUTPUT); // Applying IDFT
	invDFT.convertTo(invDFTcvt, CV_8U);
	Mat out = cut(in.rows, in.cols, invDFTcvt);
	return out;
}
Mat ideal_High_Pass_Filter(Mat in, double D0) {
	Mat img;
	img = in.clone();
	int M = getOptimalDFTSize(img.rows);
	int N = getOptimalDFTSize(img.cols);
	Mat padded;
	copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0));
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexImg;
	merge(planes, 2, complexImg);
	dft(complexImg, complexImg);
	Mat mag = complexImg;
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));
	int cx = mag.cols / 2;
	int cy = mag.rows / 2;
	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	for (int y = 0; y < mag.rows; y++) {
		double* data = mag.ptr<double>(y);
		for (int x = 0; x < mag.cols; x++) {
			double d = sqrt(pow((y - cy), 2) + pow((x - cx), 2));
			double h=1;
			if (d <= D0) {
				h= 0;
			}
			data[x] *= h;
			//std::cout << data[x] << std::endl;
		}
	}
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	Mat invDFT, invDFTcvt;
	idft(mag, invDFT, DFT_SCALE | DFT_REAL_OUTPUT); 
	/*for (int i = 0; i < invDFT.rows; i++) {
		for (int j = 0; j < invDFT.cols; j++) {
			std::cout << invDFT.at<int>(i, j) << " ";
		}
	}*/
	//imshow("test00", invDFT);
	invDFT.convertTo(invDFTcvt, CV_8U);
	Mat out = cut(in.rows, in.cols, invDFTcvt);
	return out;
}
Mat Butterworth_Low_Paass_Filter(Mat src,int n,double D0) {
	//H = 1 / (1+(D/D0)^2n)
	Mat img;
	img = src.clone();
	//调整图像加速傅里叶变换
	int M = getOptimalDFTSize(img.rows);
	int N = getOptimalDFTSize(img.cols);
	Mat padded;
	copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexImg;
	merge(planes, 2, complexImg);

	dft(complexImg, complexImg);

	Mat mag = complexImg;
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));

	int cx = mag.cols / 2;
	int cy = mag.rows / 2;

	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));

	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	for (int y = 0; y < mag.rows; y++) {
		double* data = mag.ptr<double>(y);
		for (int x = 0; x < mag.cols; x++) {
			double d = sqrt(pow((y - cy), 2) + pow((x - cx), 2));
			double h = 1.0 / (1 + pow(d / D0, 2 * n));
			//std::cout << h << std::endl;
			/*if (h <= 0.5) {
				data[x] = 0;
			}*/
			//data[x] *= (1-h);
			if (h<0.4) {
				data[x] *=0;
			}
			/*else if (h >0.1&&h<= 0.2) {
				data[x] *=0.1;
			}
			else if (h > 0.2&&h <= 0.3) {
				data[x] *= 0.2;
			}
			else if (h > 0.3&&h <= 0.4) {
				data[x] *= 0.3;
			}
			else if (h > 0.4&&h <= 0.5) {
				data[x] *= 0.4;
			}
			else if (h > 0.5&&h <= 0.6) {
				data[x] *= 0.5;
			}
			else if (h > 0.6&&h <= 0.7) {
				data[x] *= 0.6;
			}
			else if (h > 0.7&&h <= 0.8) {
				data[x] *= 0.7;
			}
			else if (h > 0.8&&h <= 0.9) {
				data[x] *= 0.8;
			}*/
			else if (h > 0.4&&h <= 0.6) {
				data[x] *= 0.5;
			}
			/*else {
				data[x] = 1;
			}*/
			//std::cout <<data[x] <<" ";
		}
	}
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	Mat invDFT, invDFTcvt;
	idft(mag, invDFT, DFT_SCALE | DFT_REAL_OUTPUT);
	/*for (int i = 0; i < invDFT.rows; i++) {
		for (int j = 0; j < invDFT.cols; j++) {
			std::cout << invDFT.at<int>(i, j) << " ";
		}
	}*/
	/*std::cout << invDFT.cols<< std::endl;
	std::cout << invDFT.rows << std::endl;*/
	invDFT.convertTo(invDFTcvt, CV_8U);
//	imshow("test", invDFT);
   /* for (int i = 0; i < invDFTcvt.rows; i++) {
		for (int j = 0; j < invDFTcvt.cols; j++) {
			std::cout <<(int) invDFTcvt.at<uchar>(i, j) << " ";
		}
	}*/
	Mat out = cut(src.rows, src.cols, invDFTcvt);
	return out;
}
Mat Butterworth_High_Paass_Filter(Mat src, int n, double D0) {
	//H = 1 / (1+(D/D0)^2n)
	Mat img;
	img = src.clone();
	//调整图像加速傅里叶变换
	int M = getOptimalDFTSize(img.rows);
	int N = getOptimalDFTSize(img.cols);
	Mat padded;
	copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexImg;
	merge(planes, 2, complexImg);

	dft(complexImg, complexImg);

	Mat mag = complexImg;
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));

	int cx = mag.cols / 2;
	int cy = mag.rows / 2;

	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));

	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	for (int y = 0; y < mag.rows; y++) {
		double* data = mag.ptr<double>(y);
		for (int x = 0; x < mag.cols; x++) {
			double d = sqrt(pow((y - cy), 2) + pow((x - cx), 2));
			double h = 1.0 / (1 + pow(d / D0, 2 * n));
			//std::cout << h << std::endl;
			/*if (h <= 0.5) {
				data[x] = 0;
			}*/
		//data[x] *= h;
		
		if (h >= 0.6) {
				data[x] *= 0;
			}
			/*else if (h >0.1&&h<= 0.2) {
				data[x] *=0.1;
			}
			else if (h > 0.2&&h <= 0.3) {
				data[x] *= 0.2;
			}
			else if (h > 0.3&&h <= 0.4) {
				data[x] *= 0.3;
			}
			else if (h > 0.4&&h <= 0.5) {
				data[x] *= 0.4;
			}
			else if (h > 0.5&&h <= 0.6) {
				data[x] *= 0.5;
			}
			else if (h > 0.6&&h <= 0.7) {
				data[x] *= 0.6;
			}
			else if (h > 0.7&&h <= 0.8) {
				data[x] *= 0.7;
			}
			else if (h > 0.8&&h <= 0.9) {
				data[x] *= 0.8;
			}*/
			
			else if (h > 0.4&&h <= 0.6) {
				data[x] *= 0.5;
			}
			
			/*else {
				data[x] = 1;
			}*/
			//std::cout <<data[x] <<" ";
		}
	}
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	Mat invDFT, invDFTcvt;
	idft(mag, invDFT, DFT_SCALE | DFT_REAL_OUTPUT);
	/*for (int i = 0; i < invDFT.rows; i++) {
		for (int j = 0; j < invDFT.cols; j++) {
			std::cout << invDFT.at<int>(i, j) << " ";
		}
	}*/
	/*std::cout << invDFT.cols<< std::endl;
	std::cout << invDFT.rows << std::endl;*/
	invDFT.convertTo(invDFTcvt, CV_8U);
//	imshow("testh", invDFT);
	/* for (int i = 0; i < invDFTcvt.rows; i++) {
		 for (int j = 0; j < invDFTcvt.cols; j++) {
			 std::cout <<(int) invDFTcvt.at<uchar>(i, j) << " ";
		 }
	 }*/
	Mat out = cut(src.rows, src.cols, invDFTcvt);
	return out;
}
Mat fuliye(Mat img) {
	Mat out;
	int m = getOptimalDFTSize(img.rows);
	int n = getOptimalDFTSize(img.cols);
	copyMakeBorder(img, out, 0, m - img.rows, 0, n - img.cols, BORDER_CONSTANT, Scalar::all(0));
	Mat outs[] = { Mat_<float>(out),Mat::zeros(out.size(),CV_32F) };
	Mat complex;
	merge(outs, 2, complex);
	dft(complex, complex);
	split(complex, outs);
	magnitude(outs[0], outs[1], outs[0]);
	Mat magI = outs[0];
	magI += Scalar::all(1);
	log(magI, magI);
	magI = magI(Rect(0, 0, magI.cols&-2, magI.rows&-2));
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));
	Mat q1(magI, Rect(cx, 0, cx, cy));
	Mat q2(magI, Rect(0, cy, cx, cy));
	Mat q3(magI, Rect(cx, cy, cx, cy));

	//变换左上角和右下角象限
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	//变换右上角和左下角象限
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	normalize(magI, magI, 0, 1, CV_MINMAX);
	return magI;
}

int main() {
	Mat img = imread("E:\\Uni\\Y3\\CV\\codes\\lab\\1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("origin", img);
	Mat mag=fuliye(img);
	imshow("fourier", mag);
	/*for (int i = 0; i < img1.cols; i++) {
		std::cout <<(int) img1.at<uchar>(152,i) << " ";
	}*/
	Mat img2=ideal_Low_Pass_Filter(img,100);
	imshow("ideal low", img2);
	Mat img3 = ideal_High_Pass_Filter(img,1);
	imshow("ideal high", img3);
	Mat img1 = Butterworth_Low_Paass_Filter(img, 2, 100);
	imshow("Butterworth low", img1);
	Mat sub = Butterworth_High_Paass_Filter(img,2,100);
	imshow("Butterworth high", sub);
	waitKey(0);
	return 0;
}