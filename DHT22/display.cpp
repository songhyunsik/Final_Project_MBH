/*
  g++ main.cpp -o display `pkg-config --cflags --libs opencv4 freetype2`
  font_type : fonts-nanum
  size : 1080*1920
*/

#include <iostream>
#include <cstdint>
#include <ctime>
#include <locale.h>
#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>

#define MAX_TIMINGS 85
#define DHT_PIN     0  /* BCM-11 */

class DHTData {
public:
    float temperature;
    float humidity;

    DHTData() : temperature(0), humidity(0) {}

    void read() {
        uint8_t laststate = HIGH;
        uint8_t counter = 0;
        uint8_t j = 0, i;
        int data[5] = { 0, 0, 0, 0, 0 };

        // Pull pin down for 18 milliseconds
        pinMode(DHT_PIN, OUTPUT);
        digitalWrite(DHT_PIN, LOW);
        delay(18);

        // Prepare to read the pin
        pinMode(DHT_PIN, INPUT);

        for (i = 0; i < MAX_TIMINGS; i++) {
            counter = 0;
            while (digitalRead(DHT_PIN) == laststate) {
                counter++;
                delayMicroseconds(1);
                if (counter == 255) break; // Timeout condition
            }
            laststate = digitalRead(DHT_PIN);

            if (counter == 255) break;

            if ((i >= 4) && (i % 2 == 0)) {
                data[j / 8] <<= 1;
                if (counter > 16) {
                    data[j / 8] |= 1;
                }
                j++;
            }
        }

        if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
            humidity = (float)((data[0] << 8) + data[1]) / 10;
            temperature = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
            if (data[2] & 0x80) {
                temperature = -temperature;
            }
        } else {
            temperature = -1;
            humidity = -1;     
        }
    }
};

void renderText(cv::Ptr<cv::freetype::FreeType2> &ft2, cv::Mat &img, const std::string &text, cv::Point pos, int fontHeight, cv::Scalar color) {
    ft2->putText(img, text, pos, fontHeight, color, -1, cv::LINE_AA, true);
}

int main() {
    setlocale(LC_ALL, "ko_KR.UTF-8");
    DHTData dht;

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
        // Read DHT data
        dht.read();

        // Create a black image
        cv::Mat display = cv::Mat::zeros(height, width, CV_8UC3);

        // Get current time
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        char date_str[100];
        char time_str[100];
        std::string humid_text = std::to_string(dht.humidity) + " %";
        std::string temp_text = std::to_string(dht.temperature) + " C";

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
        renderText(ft2, display, temp_text, cv::Point(850, 150), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, humid_text, cv::Point(850, 200), 50, cv::Scalar(255, 255, 255));
        renderText(ft2, display, "오늘 주요 기사", cv::Point(50, 1850), 50, cv::Scalar(255, 255, 255));

        cv::imshow("Information Display", display);

        if (cv::waitKey(1000) == 27) {  // Wait for 'esc' key press for 1 second
            break;
        }
    }

    return 0;
}
