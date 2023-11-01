#include"facelib.h"
extern facelib fl;


void mytest() {
	string model_name = "model";
	cout << "Read in Model..." << endl;
	FileStorage model(model_name, FileStorage::READ);
    Mat A;
    vector<Mat>allFaces;
    vector<Mat>src;
    vector<Mat>dst;
    int todet;
    int eigenCount;
	model["A"] >> A;
	model["allFaces"] >> allFaces;
    model ["eigenCount"] >> eigenCount;
    cout << "input the test object:" << endl;
    cin >> todet;
    for (int i = 6; i <= 10; i++) {
        string ima_path = "att-face/s" + to_string(todet) + "/" + to_string(i) + ".pgm";
        string eye_path = "att-eye-location/s" + to_string(todet) + "/" + to_string(i) + ".json";
        face* f = new face();
        f->load_Img(ima_path, eye_path);
        Mat testImg = f->trans;
        src.push_back(testImg);
        Mat testDoubleMat;
        testImg.reshape(0, 1).convertTo(testDoubleMat, CV_64F);
        Mat testCoordinates = testDoubleMat * A;
        double minDistance = -1;
        face resultFace;
        int id = 0;
        
        for (auto iter = allFaces.begin(); iter != allFaces.end(); iter++, id++) {
            double distance;
            face f_test;
            f_test.trans = *iter;
            Mat doubleMat;
            f_test.trans.reshape(0, 1).convertTo(doubleMat, CV_64F);
            f_test.eq = doubleMat * A;
            distance = 0;
            for (int i = 0; i < eigenCount; i++) {
                distance += pow(f_test.eq.at<double>(0, i) - testCoordinates.at<double>(0, i), 2);
            }
            if (distance < minDistance || minDistance == -1) {
                minDistance = distance;
                resultFace = f_test;
            }
        }
        dst.push_back(resultFace.trans);
    }
    Mat m1, m2, m;
    vector<Mat> fi;
    vconcat(src, m1);
    vconcat(dst, m2);
    fi.push_back(m1);
    fi.push_back(m2);
    hconcat(fi, m);
    imwrite("test result.png", m);
    imshow("test", m);
    waitKey(0);
}