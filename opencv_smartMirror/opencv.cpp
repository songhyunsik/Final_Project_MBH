#include "opencv.h"

using namespace cv;
using namespace std;

void setWinodws(Mat image,string item1,string item2,string item3,string item4,string item5,string item6,string item7){
    int font = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.8;
    int thickness = 1;
    Scalar white = Scalar(255, 255, 255);
    // Draw the text
    putText(image,item1 , Point(50, 50), font, fontScale, white, thickness);
    putText(image, item2, Point(50, 100), font, fontScale, white, thickness);
    putText(image, item3, Point(50, 150), font, fontScale, white, thickness);

    // Draw the rectangle for main news
    rectangle(image, Point(50, 450), Point(750, 550), white, 1);
    putText(image, item4, Point(60, 480), font, fontScale, white, thickness);

    // Draw the buttons and labels
    rectangle(image, Point(650, 50), Point(750, 100), white, 1);
    putText(image, item5, Point(675, 80), font, fontScale, white, thickness);

    rectangle(image, Point(650, 120), Point(700, 150), white, 1);
    putText(image, "ON", Point(660, 145), font, 0.5, white, thickness);

    rectangle(image, Point(700, 120), Point(750, 150), white, 1);
    putText(image, "OFF", Point(710, 145), font, 0.5, white, thickness);

    rectangle(image, Point(650, 180), Point(750, 230), white, 1);
    putText(image, item6, Point(670, 210), font, fontScale, white, thickness);

    rectangle(image, Point(650, 240), Point(750, 290), white, 1);
    putText(image, item7, Point(670, 270), font, fontScale, white, thickness);
}