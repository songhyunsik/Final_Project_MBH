/*
  g++ main.cpp -o display `pkg-config --cflags --libs opencv4 freetype2
  font_type : fonts-nanum
  size : 1080*1920
*/

#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include <ctime>
#include <locale.h>
#include "DHT22.h"


void renderText(cv::Ptr<cv::freetype::FreeType2> &ft2, cv::Mat &img, const std::string &text, cv::Point pos, int fontHeight, cv::Scalar color) {
    ft2->putText(img, text, pos, fontHeight, color, -1, cv::LINE_AA, true);
}

int main() {
    setlocale(LC_ALL, "ko_KR.UTF-8");

    // Initialize OpenCV window
    cv::namedWindow("Information Display", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Information Display", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    int width = 1080;
    int height = 1920;

    // Load FreeType2 font
    cv::Ptr<cv::freetype::FreeType2> ft2;
    ft2 = cv::freetype::createFreeType2();
    ft2->loadFontData("/usr/share/fonts/truetype/nanum/NanumGothic.ttf", 0);

    while (true) {
        // Create a black image
        cv::Mat display = cv::Mat::zeros(height, width, CV_8UC3);

        // Get current time
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        char date_str[100];
        char time_str[100];
        char temp_str[100] = sprintf(temp_str,"%0.1f",dhtData->temperature);
        char humi_str[100] = sprintf(humi_str,"%0.1f",dhtData->humidity);
    }
        snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        snprintf(time_str, sizeof(time_str), "%02d:%02d %s", tm.tm_hour % 12 == 0 ? 12 : tm.tm_hour % 12, tm.tm_min, tm.tm_hour >= 12 ? "PM" : "AM");

        // Render text
        renderText(ft2, display, date_str, cv::Point(50, 50), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, "세종", cv::Point(50, 100), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, "24도", cv::Point(50, 150), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, "비", cv::Point(150, 150), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, time_str, cv::Point(50, 200), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, "Room", cv::Point(850, 50), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, "ON", cv::Point(850, 100), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, "OFF", cv::Point(950, 100), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, temp_str, cv::Point(850, 150), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, humi_str, cv::Point(850, 200), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, "오늘 주요 기사", cv::Point(50, 1850), 50, cv::Scalar(255, 255, 255));

        cv::imshow("Information Display", display);

        if (cv::waitKey(1000) == 27) {  // Wait for 'esc' key press for 1 second
            break;
        }
    }

    return 0;
}
