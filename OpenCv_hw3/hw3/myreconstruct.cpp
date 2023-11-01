#include"facelib.h"
extern facelib fl;


void myreconstruct() {
    string model_name = "model";
    cout << "Read in Model..." << endl;
    FileStorage model(model_name, FileStorage::READ);
    Mat A;
    Mat eigenVectors = Mat();
    Mat mean;
    vector<Mat> allFaces;
    vector<Mat> tmp;
    Mat res;
    int eigenCount;
    model["A"] >> A;
    model["allFaces"] >> allFaces;
    model["eigenCount"] >> eigenCount;
    model["eigenVectors"] >> eigenVectors;
    model["mean"] >> mean;
    face* f = new face();
    int todet;
    int ph;
    cout << "input the person_id" << endl;
    cin >> todet;
    cout << "input the photo_id" << endl;
    cin >> ph;
    string ima_path = "att-face/s" + to_string(todet) + "/" + to_string(ph) + ".pgm";
    string eye_path = "att-eye-location/s" + to_string(todet) + "/" + to_string(ph) + ".json";
    f->load_Img("att-face/s0/1.pgm", "att-eye-location/s1/1.json");
    Mat testDoubleMat;
    Mat testImg = f->trans;

    testImg.reshape(0, 1).convertTo(testDoubleMat, CV_64F);
    for (int i = 0; i < 5 ;i++) {
        int num_components;
        if (i == 0) num_components = 10;
        else if (i == 1) num_components = 25;
        else if (i == 2) num_components = 50;
        else if (i == 3) num_components = 75;
        else num_components = 585;
        Mat evs = Mat(A, Range::all(), Range(0, num_components));
        Mat testCoordinates = testDoubleMat * evs;
        Mat recst = testCoordinates * (evs.t());
        normalize(recst.reshape(1, f->trans.rows), recst, 255, 0, NORM_MINMAX);
        recst.convertTo(recst, CV_8UC1);
        tmp.push_back(recst);
    }
    hconcat(tmp, res);
    imwrite("reconstruct.png", res);
    imshow("res", res);
    waitKey(0);

}
