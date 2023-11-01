#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
using namespace std;
using namespace cv;
const int WIDTH = 25;
const int HEIGHT = 25;
class face {
public:
	string img_Path;
	string eye_Path;
	Mat src;
	Mat gray;
	Mat handle;
	Mat trans;
	Mat eq;
	Mat vect;
	int eyelx, eyely, eyerx, eyery;
	string person_id;
	void load_Img(string _imgpath, string _eyepath);
	void load_Eye(string path);
};

class facelib {
public:
	int Size;
	vector<face*>entrys;
	vector<Mat> _samples;
	vector<Mat> sam;
	Mat samples;

	void load(string pathi ,string pathe);
	~facelib() {
		for (int i = 0; i < entrys.size(); i++) {
			face* f = entrys.back();
			entrys.pop_back();
			delete(f);
		}
	}
};

void mytrain();
void mytest();
void myreconstruct();
Mat toImg(Mat vec, int w, int h);
string getIDfrompath(string s);