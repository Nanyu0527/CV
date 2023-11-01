#include<opencv2/opencv.hpp>
#include<iostream>
#include <stdlib.h>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <ctype.h>
using namespace cv;
using namespace std;

vector<Point> op_Frame(Mat srcImage);
void get_Sobel(Mat& imageSource, Mat& imageSobelX, Mat& imageSobelY);
void Sobel2(const Mat imageGradX, Mat_<float>& SobelAmpXX); 
void SobelXY(const Mat imageGradX, const Mat imageGradY, Mat_<float>& SobelAmpXY);
void harris_G(Mat_<float>& GaussXX, Mat_<float>& GaussYY, Mat_<float>& GaussXY, Mat_<float>& resultData, float k);
vector<Point> LocalMaxValue(Mat_<float>& resultData, Mat& srcGray, int kSize);
void print_Point(vector<Point> v, Mat& ResultImage);
Mat grayToRGB(const Mat input_img);
//Mat res;
int main()
{
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		cout << "Can't load the Cam" << endl;
		cap.open("arc.mp4");
	}
	if (!cap.isOpened()) {
		cout << "Can't load the video" << endl;
		return -1;
	}
	int flag = 0;
	vector<Point> v;
	Mat frame;
	Mat photo;
	while (cap.isOpened()) {

		bool success = cap.read(frame);
		if (success == true) {
			int key = waitKey(24);
			if (key == 'p') {
				flag = 1;
				v = op_Frame(frame);
				frame.copyTo(photo);
				print_Point(v, photo);
			}
			imshow("Frame", frame);
			if (flag != 0) {
				imshow("Photo", photo);
			}
		}
		else {
			cout << "disconnected" << endl;
			break;
		}
		int key = waitKey(24);
		if (key == 'q') {
			break;
		}
	}

	cap.release();
	destroyAllWindows();
	return 0;
}
vector<Point> op_Frame(Mat srcImage) {
	
	Mat srcGray;
	cvtColor(srcImage, srcGray, COLOR_BGR2GRAY);
	//ConvertRGB2GRAY(srcImage, srcGray);
	Mat imageSobelX;// = Mat::zeros(srcGray.size(), CV_32SC1);
	Mat imageSobelY;
	Mat resultImage;
	Mat_<float> imageSobelXX;
	Mat_<float> imageSobelYY;
	Mat_<float> imageSobelXY;
	Mat_<float> GaussianXX;
	Mat_<float> GaussianYY;
	Mat_<float> GaussianXY;
	Mat_<float> HarrisRespond;
	Mat_<float> res;
	Mat_<float> R_graph;
	//¼ÆËãSobleµÄXYÌÝ¶È
	Sobel(srcGray, imageSobelX, CV_8UC1, 1, 0, 3);
	convertScaleAbs(imageSobelX, imageSobelX);
	Sobel(srcGray, imageSobelY, CV_8UC1, 0, 1, 3);
	convertScaleAbs(imageSobelY, imageSobelY);
	Sobel2(imageSobelX, imageSobelXX);
	Sobel2(imageSobelY, imageSobelYY);
	SobelXY(imageSobelX, imageSobelY, imageSobelXY);
	GaussianBlur(imageSobelXX, GaussianXX, { 3,3 }, 1);
	GaussianBlur(imageSobelYY, GaussianYY, { 3,3 }, 1);
	GaussianBlur(imageSobelXY, GaussianXY, { 3,3 }, 1);

	harris_G(GaussianXX, GaussianYY, GaussianXY, HarrisRespond, 0.05);
	resultImage = srcGray.clone();
	vector<Point> v_P = LocalMaxValue(res, HarrisRespond, 3);

	return v_P;
}
void get_Sobel(Mat& imageSource, Mat& imageSobelX, Mat& imageSobelY) {
	imageSobelX = Mat::zeros(imageSource.size(), CV_32SC1);
	imageSobelY = Mat::zeros(imageSource.size(), CV_32SC1);
}
void Sobel2(const Mat src, Mat_<float>& dst) {
	dst = Mat(src.size(), CV_32FC1);
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			//src.at<int>(i, j);
			dst.at<float>(i, j) = src.at<uchar>(i, j) * src.at<uchar>(i, j);
		}
	}
}
void SobelXY(const Mat src1, const Mat src2, Mat_<float>& dst) {
	dst = Mat(src1.size(), CV_32FC1);
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			dst.at<float>(i, j) = src1.at<uchar>(i, j)*src2.at<uchar>(i,j);
		}
	}
}
void harris_G(Mat_<float>& GaussXX, Mat_<float>& GaussYY, Mat_<float>& GaussXY, Mat_<float>& res, float k) {
	res = Mat_<float>(GaussXX.size(), CV_32FC1);
	for (int i = 0; i < res.rows; i++) {
		for (int j = 0; j < res.cols; j++) {
			float x = GaussXX.at<float>(i, j);
			float y = GaussYY.at<float>(i, j);
			float z = GaussXY.at<float>(i, j);
			res.at<float>(i, j) = x * y - z * z - k * (x + y) * (x + y);
		}
	}
}
vector<Point> LocalMaxValue(Mat_<float>& res, Mat& src, int kSize) {
	int l = kSize / 2;
	res = src.clone();
	vector <Point> v;
	for (int i = l; i < res.rows - l; i++) {
		for (int j = l; j < res.cols - l; j++) {
			if (res.at<float>(i, j) > res.at<float>(i - 1, j - 1) &&
				res.at<float>(i, j) > res.at<float>(i - 1, j) &&
				res.at<float>(i, j) > res.at<float>(i - 1, j - 1) &&
				res.at<float>(i, j) > res.at<float>(i - 1, j + 1) &&
				res.at<float>(i, j) > res.at<float>(i, j - 1) &&
				res.at<float>(i, j) > res.at<float>(i, j + 1) &&
				res.at<float>(i, j) > res.at<float>(i + 1, j - 1) &&
				res.at<float>(i, j) > res.at<float>(i + 1, j) &&
				res.at<float>(i, j) > res.at<float>(i + 1, j + 1)) {
				if ((int)res.at<float>(i, j) > 18000) {
					v.push_back ( Point(j, i));
				}
			}
		}
	}
	return v;
}
void print_Point(vector<Point> v , Mat& ResultImage) {
	for (int i = 0; i < v.size(); i++) {
		circle(ResultImage, Point(v[i].x, v[i].y), 2, Scalar(0, 0, 0), 2, 8, 0);
	}
}
Mat grayToRGB(const Mat input_img)
{
	cv::Mat three_channel = cv::Mat::zeros(input_img.rows, input_img.cols, CV_8UC3);
	std::vector<cv::Mat> channels;
	for (int i = 0; i < 3; i++)
		channels.push_back(input_img);
	cv::merge(channels, three_channel);
	return three_channel;
}