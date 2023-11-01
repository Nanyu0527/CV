#include"facelib.h"

using namespace cv;
using namespace std;
facelib fl;

int main() {
    int type = 0;
    cout << "input the type: \ntrain:1\ttest:2\treconstruct:3" << endl;
    cin >> type;
    if (type == 1) {
        mytrain();
    }
    else if (type == 2) {
        mytest();
    }
    else if (type == 3) {
        myreconstruct();
    }
    else {
        cout << "invalid type" << endl;
    }
}