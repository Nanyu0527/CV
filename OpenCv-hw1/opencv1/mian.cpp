#include<opencv2/opencv.hpp>
#include<iostream>
#include <stdlib.h>
#include <stdio.h>
using namespace cv;
using namespace std;

Mat get_Title(Mat& background);
Mat get_resize(Mat background);
Mat get_start(Mat background);
void resize_Demo(Mat background);
void draw_pics(Mat& background);
void end_pics(Mat& background);
void draw_Animation(Mat& background);
void draw_dialog(Mat& background,string text,int rule);
void push_cg(Mat src1, Mat src2);
void clarity_cg(Mat src1, Mat src2);
const int WINDOW_SCALE_WIDTH = 960;
const int WINDOW_SCALE_HEIGHT = 540;
VideoWriter writer;
int main() {
	Mat m = Mat::zeros(Size(WINDOW_SCALE_WIDTH, WINDOW_SCALE_HEIGHT), CV_8UC3);
	int frameHeight = m.rows;
	int frameWidth = m.cols;
	int fps = 30;
	writer = VideoWriter("hw_l.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps,
		Size(frameWidth, frameHeight), 1);
	namedWindow("hw1", WINDOW_NORMAL);
	resizeWindow("hw1", WINDOW_SCALE_WIDTH, WINDOW_SCALE_HEIGHT);
	//imshow("hw1", m);
	
	Mat m1 = get_Title(m);
	//resize_Demo(m);
	Mat m2 = get_resize(m);
	Mat m3 = get_start(m);
	imshow("hw1", m1);
	for (int i = 0; i < 60; i++) {
		writer.write(m1);
	}
	waitKey(2000);
	push_cg(m1, m2);
	imshow("hw1", m2);
	for (int i = 0; i < 60; i++) {
		writer.write(m2);
	}
	waitKey(2000);
	//push_cg(m1, m2);
	clarity_cg(m2, m3);
	imshow("hw1", m3);
	for (int i = 0; i < 30; i++) {
		writer.write(m3);
	}
	waitKey(1000);

	draw_pics(m);
	draw_Animation(m);
	end_pics(m);

	destroyAllWindows();
	return 0;

}
void push_cg(Mat src1, Mat src2) {
	int t = 0;
	for (int i = 0; i < src1.cols; i+=5) {
		Mat imageROI1 = src1(Rect(0, 0, i, src1.rows));//加mask
		Mat imageROI2 = src2(Rect(0, 0, i, src2.rows));
		imageROI2.copyTo(imageROI1);
		imshow("hw1", src1);
		if (t == 4) {
			writer.write(src1);
			t = 0;
		}
		t++;
		waitKey(10);
	}
	//waitKey(0);
}
void clarity_cg(Mat src1, Mat src2) {
	for (int i = 0; i < 100; i+=5) {
		Mat overlap;
		addWeighted(src2, i * 1.0 / 100, src1, (100 - i) * 1.0 / 100, 0, overlap);
		imshow("hw1", overlap);
		writer.write(overlap);
		waitKey(10);
	}
	//waitKey(0);
}
void draw_Animation(Mat& background) {
	string src1 = "海绵宝宝.png";
	string src2 = "派大星.png";
	string src3 = "bg.jpeg";
	Mat m = imread(src3);
	if (m.empty()) {
		printf("can't download");
		return;
	}
	Mat m1 = imread(src1);
	if (m1.empty()) {
		printf("can't download");
		return ;
	}
	Mat m2 = imread(src2);
	if (m2.empty()) {
		printf("can't download");
		return;
	}
	resize(m, m, Size(WINDOW_SCALE_WIDTH, WINDOW_SCALE_HEIGHT), 0, 0, INTER_LINEAR);
	int h = m1.rows;
	int w = m1.cols;
	//background.setTo(m);
	Mat mask1 = imread(src1);
	Mat mask2 = imread(src2);
	Mat imageROI1, imageROI2, imageROI;
	imageROI = background(Rect(0, 0, m.cols, m.rows));
	m.copyTo(imageROI);
	for (int i = 0; i < 140; i++) {
		m.copyTo(imageROI);//any better idea?
		imageROI1 = background(Rect(i*2, 300, m1.cols, m1.rows));
		imageROI2 = background(Rect(background.cols - 200-i*2, 300, m2.cols, m2.rows));
		m1.copyTo(imageROI1, mask1);
		m2.copyTo(imageROI2, mask2);
		waitKey(10);
		imshow("hw1", background);
		writer.write(background);
	}
	m.copyTo(imageROI);
	m1.copyTo(imageROI1, mask1);
	m2.copyTo(imageROI2, mask2);
	Mat dst;
	imageROI.copyTo(dst);
	draw_dialog(background,"Friend, we seem to have drifted apart",1);
	dst.copyTo(imageROI);
	draw_dialog(background, "What ? What do you mean ?", 0);
	dst.copyTo(imageROI);
	draw_dialog(background, "We used to be very happy together", 1);
	dst.copyTo(imageROI);
	draw_dialog(background, "But now we are not compatible", 1);
	dst.copyTo(imageROI);
	draw_dialog(background, "We should go our separate ways.", 1);
	dst.copyTo(imageROI);
	draw_dialog(background, "This is life.", 1);
	dst.copyTo(imageROI);
	draw_dialog(background, "But you're my best friend!", 0);
	dst.copyTo(imageROI);
	draw_dialog(background, "I know it's hard, let's face it", 1);
	dst.copyTo(imageROI);
	draw_dialog(background, "Maybe we will meet again in the future.", 1);
	dst.copyTo(imageROI);
	draw_dialog(background, "Please don't forget me", 1);
	dst.copyTo(imageROI);
	draw_dialog(background, "Mr. SpongeBob SquarePants.", 1);
	dst.copyTo(imageROI);
	draw_dialog(background, "wwwwwwwwwwwwwwwww", 0);
	for (int i = 139; i >=0 ; i--) {
		m.copyTo(imageROI);//any better idea?
		imageROI1 = background(Rect(i * 2, 300, m1.cols, m1.rows));
		imageROI2 = background(Rect(background.cols - 200 - i * 2, 300, m2.cols, m2.rows));
		m1.copyTo(imageROI1, mask1);
		m2.copyTo(imageROI2, mask2);
		waitKey(10);
		imshow("hw1", background);
		writer.write(background);
	}
	for (int i = 0; i < 30; i++) {
		writer.write(background);
	}
	waitKey(1000);
}
void draw_dialog(Mat& background, string text, int rule) {
	
	int font_face = FONT_HERSHEY_COMPLEX;
	double font_scale = 0.6;
	int thinkness = 0.5;
	int baseline;
	Size text_size = getTextSize(text, font_face, font_scale, thinkness, &baseline);
	Point origin;
	origin.x = rule==1?500:400-text_size.width;
	origin.y = 280;
	putText(background, text, origin, font_face, font_scale, Scalar(255, 255, 255), thinkness, 8, 0);
	imshow("hw1", background);
	for (int i = 0; i < 60; i++) {
		writer.write(background);
	}
	waitKey(2000);
}
void end_pics(Mat& background) {
	background.setTo(Scalar(202, 235, 216));
	const string text[] = {"Material: SpongeBob SquarePants","Production Language: C++/OpenCV",
		 "Instructor: Pan Gang","Major: Computer Science and Technology","Author: Cao Xiaochuan" };
	const string t_End = "Thank you";
	int font_face = FONT_HERSHEY_COMPLEX;
	double font_scale = 1;
	int thinkness = 2;
	int baseline;
	const int dis = 50;
	const int speed = 3;
	int Time = 0;
	while (1) {
		for (int i = 0; i < 5; i++) {
			Size text_size = getTextSize(text[i], font_face, font_scale, thinkness, &baseline);
			Point origin;
			origin.x = background.cols / 2 - text_size.width / 2;
			origin.y = background.rows - speed * Time - dis * i;
			putText(background, text[i], origin, font_face, font_scale, Scalar(0, 255, 255), thinkness, 8, 0);
		}
		Time++;
		waitKey(10);
		imshow("hw1", background);
		writer.write(background);
		background.setTo(Scalar(202, 235, 216));
		if (background.rows - speed * Time  <= 0) {
			break;
		}
	}
	waitKey(100);
	for (int i = 0; i < 10; i++) {
		writer.write(background);
	}
	font_scale = 2;
	Size text_size = getTextSize(t_End, font_face, font_scale, thinkness, &baseline);
	Point origin;
	origin.x = background.cols / 2 - text_size.width / 2;
	origin.y = background.rows / 2 + text_size.height / 2;
	putText(background, t_End, origin, font_face, font_scale, Scalar(0, 255, 255), thinkness, 8, 0);
	imshow("hw1", background);
	for (int i = 0; i < 60; i++) {
		writer.write(background);
	}
	waitKey(2000);
}
void draw_pics(Mat& background) {
	background.setTo(Scalar(202, 235, 216));
	enum pic_type { LINE,ELLIPSE,RECTANGLE,TRIANGLE};
	RNG rng(12345);//产生随机数
	int pic_num = 0;
	while (true) {
		waitKey(50);
		pic_num++;
		if (pic_num == 100) {
			break;
		}
		int type = rng.uniform(0, 4);
		if (type == LINE) {
			Point p1;
			p1.x = rng.uniform(0, background.cols);
			p1.y = rng.uniform(0, background.rows);
			Point p2;
			p2.x = rng.uniform(0, background.cols);
			p2.y = rng.uniform(0, background.rows);
			line(background, p1, p2, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2, LINE_AA, 0);
		}
		else if (type == ELLIPSE) {
			Point e_Size;
			e_Size.x = rng.uniform(20, 200);
			e_Size.y = rng.uniform(20, 200);
			Point center;
			center.x = rng.uniform(e_Size.x, background.cols - e_Size.x);
			center.y = rng.uniform(e_Size.y, background.rows - e_Size.y);
			ellipse(background, center, e_Size, 0, 0, 360, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 3, LINE_AA, 0);
		}
		else if (type == RECTANGLE) {
			Point center;
			Point r_Size;
			r_Size.x = rng.uniform(20, 200);
			r_Size.y = rng.uniform(20, 200);
			center.x = rng.uniform(0, background.cols - r_Size.x);
			center.y = rng.uniform(0, background.rows - r_Size.y);
			rectangle(background, center, r_Size, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 3, LINE_AA, 0);

		}
		else if (type == TRIANGLE) {
			Point p1, p2, p3;
			p1.x = rng.uniform(0, background.cols);
			p1.y = rng.uniform(0, background.rows);
			p2.x = rng.uniform(0, background.cols);
			p2.y = rng.uniform(0, background.rows);
			p3.x = rng.uniform(0, background.cols);
			p3.y = rng.uniform(0, background.rows);
			vector<Point> pts;
			pts.push_back(p1);
			pts.push_back(p2);
			pts.push_back(p3);
			polylines(background, pts, true, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 3, LINE_AA, 0);
		}		
		imshow("hw1", background);
		for (int i = 0; i < 3; i++) {
			writer.write(background);
		}
		waitKey(10);
	}
	for (int i = 0; i < 30; i++) {
		writer.write(background);
	}
	waitKey(1000);
	Mat m = imread("bg.jpeg");
	resize(m, m, Size(WINDOW_SCALE_WIDTH, WINDOW_SCALE_HEIGHT), 0, 0, INTER_LINEAR);
	if (m.empty()) {
		printf("can't download");
		return;
	}
	push_cg(background, m);
}
Mat get_Title(Mat& background) {
	background.setTo(Scalar(202, 235, 216));
	string text = "Hello guys";
	int font_face = FONT_HERSHEY_COMPLEX;
	double font_scale = 2;
	int thinkness = 2;
	int baseline;
	Size text_size = getTextSize(text, font_face, font_scale, thinkness, &baseline);
	Point origin;
	origin.x = background.cols / 2 - text_size.width / 2;
	origin.y = background.rows / 2 + text_size.height / 2;
	putText(background, text, origin, font_face, font_scale, Scalar(0, 255, 255), thinkness, 8, 0);
	imshow("hw1", background);
	writer.write(background);
	return background;
}
Mat get_resize(Mat in) {
	Mat background;
	in.copyTo(background);
	Mat image = imread("head.jpg");
	if (image.empty()) {
		printf("can't download");
		return in;
	}
	int h = image.rows / 4;
	int w = image.cols / 4;
	background.setTo(Scalar(251, 255, 242));
	Mat imageROI = background(Rect(background.rows / 2 - w / 2, 60, w, h));
	resize(image, image, Size(w, h), 0, 0, INTER_LINEAR);
	image.copyTo(imageROI);//这里不能用background
	string text1 = "name : Xiaochuan Cao";
	string text2 = "Student ID : 3200105705";

	int font_face = FONT_HERSHEY_COMPLEX;
	double font_scale = 1;
	int thinkness = 1;
	int baseline;
	Size text_size = getTextSize(text1, font_face, font_scale, thinkness, &baseline);
	Point origin;
	origin.x = background.cols / 2 + w / 2 - text_size.width / 3;
	origin.y = 220;
	putText(background, text1, origin, font_face, font_scale, Scalar(205, 224, 64), thinkness, 8, 0);
	imshow("hw1", background);
	writer.write(background);
	text_size = getTextSize(text2, font_face, font_scale, thinkness, &baseline);
	//origin.x = background.cols / 2 - text_size.width / 2;
	origin.y = 350;
	putText(background, text2, origin, font_face, font_scale, Scalar(205, 224, 64), thinkness, 8, 0);
	//imshow("hw1", background);
	//waitKey(3000);
	return background;
}
Mat get_start(Mat in) {
	Mat background;
	in.copyTo(background);
	string text = "Start my performance";
	background.setTo(Scalar(251, 255, 242));
	//background.setTo(Scalar(0, 0, 0));
	Point origin;
	int font_face = FONT_HERSHEY_COMPLEX;
	double font_scale = 1.5;
	int thinkness = 1;
	int baseline;
	Size text_size = getTextSize(text, font_face, font_scale, thinkness, &baseline);
	origin.x = background.cols / 2 - text_size.width / 2;
	origin.y = background.rows / 2 + text_size.height / 2;
	putText(background, text, origin, font_face, font_scale, Scalar(255, 255, 0), thinkness, 8, 0);
	//imshow("hw1", background);
	//waitKey(1000);
	return background;
}
void resize_Demo(Mat& background) {
	Mat image = imread("head.jpg");
	if (image.empty()) {
		printf("can't download");
		return ;
	}
	int h = image.rows/4;
	int w = image.cols/4;
	background.setTo(Scalar(251, 255, 242));
	Mat imageROI = background(Rect(background.rows/2 - w/2, 60, w, h));
	Mat mask(h, w, CV_8UC3, Scalar(1, 1, 1));
	resize(image, image, Size(w ,h), 0, 0, INTER_LINEAR);
	image.copyTo(imageROI);//这里不能用background
	string text1 = "name : Xiaochuan Cao";
	string text2 = "Student ID : 3200105705";
	string text3 = "Start my performance";
	int font_face = FONT_HERSHEY_COMPLEX;
	double font_scale = 1;
	int thinkness = 1;
	int baseline;
	Size text_size = getTextSize(text1, font_face, font_scale, thinkness, &baseline);
	Point origin;
	origin.x = background.cols / 2 + w / 2 - text_size.width / 3;
	origin.y = 220;
	putText(background, text1, origin, font_face, font_scale, Scalar(205,224,64), thinkness, 8, 0);
	imshow("hw1", background);
	writer.write(background);
	text_size = getTextSize(text2, font_face, font_scale, thinkness, &baseline);
	//origin.x = background.cols / 2 - text_size.width / 2;
	origin.y = 350;
	putText(background, text2, origin, font_face, font_scale, Scalar(205, 224, 64), thinkness, 8, 0);
	imshow("hw1", background);
	for (int i = 0; i < 90; i++) {
		writer.write(background);
	}
	waitKey(3000);
	background.setTo(Scalar(251, 255, 242));
	//background.setTo(Scalar(0, 0, 0));
	font_scale = 2;
	thinkness = 2;
	text_size = getTextSize(text3, font_face, font_scale, thinkness, &baseline);
	origin.x = background.cols / 2 - text_size.width / 2;
	origin.y = background.rows / 2 + text_size.height / 2;
	putText(background, text3, origin, font_face, font_scale, Scalar(255, 255, 0), thinkness, 8, 0);
	imshow("hw1", background);
	for (int i = 0; i < 30; i++) {
		writer.write(background);
	}
	waitKey(1000);
}