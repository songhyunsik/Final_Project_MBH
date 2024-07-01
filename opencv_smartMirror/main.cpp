#include "opencv.h"
#include <string>
using namespace cv;
using namespace std;

int main() {
    Mat image = Mat::zeros(600, 800, CV_8UC3);
    
    // define data
    std::string item1 ="(Date)  (Time)";
    std::string item2  ="(resion)";
    std::string item3 ="(weather)";
    std::string item4 ="(RoomIOT)";
    std::string item5 ="(Today's)";
    std::string item6 ="(Temp)";
    std::string item7 ="(humi)";

    // display placing
    setWinodws(image, item1,item2,item3,item4,item5,item6,item7);

    imshow("Interface", image);
    waitKey(0);

    return 0;
}


