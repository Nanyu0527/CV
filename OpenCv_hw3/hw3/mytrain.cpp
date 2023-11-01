#include"facelib.h"


string picpath = "att-face";
string eyepath = "att-eye-location";
Size standardSize = Size(25, 25);
extern facelib fl;

string getIDfrompath(string s) {
	string id = "";
	for (int i = 0; i < s.length(); i++) {
		while (s[i] != 's' && i < s.length()) { i++; }
		while (s[i] != '\\' && i < s.length()) {
			id += s[i];
			i++;
		}
		break;
	}
	return id;
}

Mat toImg(Mat vec, int w, int h){
	assert(vec.type() == 6);
	assert(vec.cols == w * h);
	Mat res(Size(w, h), CV_64FC1);
	for (int i = 0; i < h; i++) {
		vec.colRange(i * w, (i + 1) * w).convertTo(res.row(i), CV_64FC1);//转化为一列
	}
	normalize(res, res, 1.0, 0.0, NORM_MINMAX);
	return res;
}
void mytrain() {
	string model_name = "model";
	double energy = 0.95;
	cout << "Read in facelib..." << endl;
	fl.load(picpath,eyepath);
	Mat samples, mmat ,cmat; //输出的协方差矩阵,均值矩阵
	fl.samples.copyTo(samples);
	cout << samples.size() << endl;
	cout << "Caculating Covariance Mat..." << endl;
	calcCovarMatrix(fl.sam, cmat, mmat, CV_COVAR_ROWS | CV_COVAR_NORMAL);
	Mat meanImg;
	mmat.convertTo(meanImg, CV_8UC1);
	imwrite("mean_img.png", meanImg);
	cout << "cmat" << cmat.type() << endl;
	cout << "cmat" << cmat.size() << endl;
	cout << "mmat" << mmat.size() << endl;
	Mat eigenVectors = Mat();
	Mat eigenValues = Mat();
	vector<Mat> eigenFaces;
	eigen(cmat, eigenValues, eigenVectors);
	cout << eigenVectors.size() << endl;
	double energyPercent = 0.95;
	int eigenCount = (int)(eigenVectors.rows * energyPercent);
	cout << "eigen count: " << eigenCount << endl;
	Mat AT = Mat(eigenCount, standardSize.height * standardSize.width, CV_64F);
	Mat A = Mat(standardSize.height * standardSize.width, eigenCount, CV_64F);//特征向量
	for (int i = 0; i < eigenCount; i++) {
		Mat t = Mat(standardSize.height, standardSize.width, CV_64F);
		Mat tt = Mat(standardSize.height, standardSize.width, CV_8UC1);
		for (int j = 0; j < standardSize.width * standardSize.height; j++) {
			t.at<double>(j / standardSize.width, j % standardSize.width) = eigenVectors.at<double>(i, j);
			AT.at<double>(i, j) = eigenVectors.at<double>(i, j);
			A.at<double>(j, i) = eigenVectors.at<double>(i, j);
		}
		normalize(t, t, 255, 0, NORM_MINMAX);
		t.convertTo(tt, CV_8UC1);
		eigenFaces.push_back(tt);
	}

	cout << "Write Model..." << endl;
	FileStorage model(model_name, FileStorage::WRITE);
	model << "eigenCount" << eigenCount;
	model << "allFaces" << fl.sam;
	model << "A" << A;
	model << "eigenVectors" << eigenVectors;
	model << "mean" << mmat;
	model.release();

	Mat tmp1,tmp,tmp2;
	vector<Mat> t1, t2,t;
	for (int i = 0; i < 5 && i < eigenCount; i++) {
		t1.push_back(eigenFaces.at(i));
	}
	hconcat(t1, tmp1);
	t.push_back(tmp1);
	cout << 123 << endl;
	for (int i = 5; i < 10 && i < eigenCount; i++) {
		t2.push_back(eigenFaces.at(i));
	}
	hconcat(t2, tmp2);
	t.push_back(tmp2);
	vconcat(t, tmp);
	imwrite("top10_img.png", tmp);
	imshow("Top10", tmp);
	waitKey(0);
}

void face::load_Img(string _imgpath, string _eyepath) {
	img_Path = _imgpath;
	eye_Path = _eyepath;
	load_Eye(_eyepath);
	src = imread(img_Path);
	cvtColor(src, gray, COLOR_RGB2GRAY);
	Point mid;
	mid.x = (eyelx + eyerx) / 2;
	mid.y = (eyely + eyery) / 2;
	double angle = atan((eyery - eyely) * 1.0 / (eyerx - eyelx) * 1.0) * 180 / CV_PI;
	handle = getRotationMatrix2D(mid, angle, 1.0);
	warpAffine(gray, trans, handle, gray.size());
	equalizeHist(trans, trans);//直方图均衡化是通过拉伸像素强度分布范围来增强图像对比度的一种方法
	resize(trans, trans, standardSize);
	trans.copyTo(eq);
	vect = eq.reshape(1, 1).t();//转置
	string name="";
	person_id = getIDfrompath(_imgpath);
}
void face::load_Eye(string path) {
	ifstream file(path.c_str());
	string s1, s2, s3, s4;
	string Read;
	if(!file){
		cout << "Can Load Eye file" << endl;
		return;
	}
	string line1;
	string line2;
	getline(file, line1);
	getline(file, line1);

	getline(file, line1);
	for (int i = 0; i < line1.size(); i++) {
		if (line1[i] >= '0' && line1[i] <= '9') {
			eyelx = eyelx * 10 + line1[i] - '0';
		}
	}
	getline(file, line2);
	for (int i = 0; i < line2.size(); i++) {
		if (line2[i] >= '0' && line2[i] <= '9') {
			eyely = eyely * 10 + line2[i] - '0';
		}
	}
	getline(file, line1);
	getline(file, line1);
	getline(file, line1);
	for (int i = 0; i < line1.size(); i++) {
		if (line1[i] >= '0' && line1[i] <= '9') {
			eyerx = eyerx * 10 + line1[i] - '0';
		}
	}
	getline(file, line2);
	for (int i = 0; i < line2.size(); i++) {
		if (line2[i] >= '0' && line2[i] <= '9') {
			eyery = eyery * 10 + line2[i] - '0';
		}
	}
	file.close();

	
}
void facelib::load(string pathi,string pathe) {
	vector<String> img1, img2, img3, img4, img5,img;
	vector<String> eye1, eye2, eye3, eye4, eye5,eye;
	glob(pathi + "/1.pgm", img1, true);
	glob(pathe + "/1.json", eye1, true);
	glob(pathi + "/2.pgm", img2, true);
	glob(pathe + "/2.json", eye2, true);
	glob(pathi + "/3.pgm", img3, true);
	glob(pathe + "/3.json", eye3, true);
	glob(pathi + "/4.pgm", img4, true);
	glob(pathe + "/4.json", eye4, true);
	glob(pathi + "/5.pgm", img5, true);
	glob(pathe + "/5.json", eye5, true);
	
	img.insert(img.end(), img1.begin(), img1.end());
	img.insert(img.end(), img2.begin(), img2.end());
	img.insert(img.end(), img3.begin(), img3.end());
	img.insert(img.end(), img4.begin(), img4.end());
	img.insert(img.end(), img5.begin(), img5.end());

	eye.insert(eye.end(), eye1.begin(), eye1.end());
	eye.insert(eye.end(), eye2.begin(), eye2.end());
	eye.insert(eye.end(), eye3.begin(), eye3.end());
	eye.insert(eye.end(), eye4.begin(), eye4.end());
	eye.insert(eye.end(), eye5.begin(), eye5.end());

	if (img.size() == 0 || eye.size() == 0) {
		cout << "no such files" << endl;
		return;
	}
	else if (img.size() != eye.size()) {
		cout << "imgs and eyes are not fit" << endl;
		return;
	}
	
	sort(img.begin(), img.end());
	sort(eye.begin(), eye.end());

	Size = img.size();
	for (int i = 0; i < img.size(); i++) {
		face *f = new face();
		f ->load_Img(img.at(i), eye.at(i));
		entrys.push_back(f);
		sam.push_back(f->trans);
		_samples.push_back(f->vect);//列向量
	}
	hconcat(_samples, samples);
	cout << "Size of facelib is " << Size << endl;
	Mat m;
	samples.convertTo(m, CV_8U);
	imwrite("face0_trans.png", entrys.at(0)->trans);
	
}