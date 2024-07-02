/*
compile:
    g++ -o dashboard initDisplay.cpp `pkg-config --cflags --libs opencv4`
download hangle font:
    sudo apt update
    sudo apt install fonts-nanum
path:
    /usr/share/fonts/truetype/nanum/
*/

#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include <ctime>
#include <string>
#include <iostream>

void drawText(cv::Ptr<cv::freetype::FreeType2> ft2, cv::Mat &image, const std::string &text, cv::Point org, int fontHeight, cv::Scalar color) {
    ft2->putText(image, text, org, fontHeight, color, -1, cv::LINE_AA, true);
}

void drawButton(cv::Mat &image, const std::string &text, cv::Point top_left, cv::Point bottom_right, cv::Scalar color, cv::Scalar textColor, cv::Ptr<cv::freetype::FreeType2> ft2) {
    cv::rectangle(image, top_left, bottom_right, color, cv::FILLED);
    int baseline = 0;
    int fontHeight = 12;
    cv::Size textSize = ft2->getTextSize(text, fontHeight, -1, &baseline);
    cv::Point textOrg((top_left.x + bottom_right.x - textSize.width) / 2, (top_left.y + bottom_right.y + textSize.height) / 2);
    ft2->putText(image, text, textOrg, fontHeight, textColor, -1, cv::LINE_AA, true);
}

std::string getCurrentDate() {
    std::time_t t = std::time(nullptr);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", std::localtime(&t));
    return std::string(buffer);
}

std::string getCurrentTime() {
    std::time_t t = std::time(nullptr);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%p, %I:%M:%S", std::localtime(&t));
    return std::string(buffer);
}

int main() {
    // FreeType2 초기화
    cv::Ptr<cv::freetype::FreeType2> ft2 = cv::freetype::createFreeType2();
    std::string fontPath = "/usr/share/fonts/truetype/nanum/NanumGothic.ttf";  // 폰트 파일 경로

    try {
        ft2->loadFontData(fontPath, 0);  // 폰트 로드
    } catch (const cv::Exception &e) {
        std::cerr << "Error: Could not load font at " << fontPath << std::endl;
        std::cerr << e.what() << std::endl;
        return -1;
    }

    // 대시보드 매트릭스 생성
    cv::Mat dashboard(600, 300, CV_8UC3, cv::Scalar(0, 0, 0));

    // 창 생성 및 풀스크린 설정
    cv::namedWindow("Dashboard", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Dashboard", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    while (true) {
        // 현재 날짜와 시간 가져오기
        std::string currentDate = getCurrentDate();
        std::string currentTime = getCurrentTime();

        // 대시보드 초기화
        dashboard.setTo(cv::Scalar(0, 0, 0));

        // 날짜와 위치 표시
        drawText(ft2, dashboard, currentDate, cv::Point(10, 30), 20, cv::Scalar(255, 255, 255));
        drawText(ft2, dashboard, "세종", cv::Point(10, 60), 20, cv::Scalar(255, 255, 255));

        // 온도와 날씨 표시
        drawText(ft2, dashboard, "24도", cv::Point(10, 90), 20, cv::Scalar(255, 255, 255));
        drawText(ft2, dashboard, "비", cv::Point(60, 90), 20, cv::Scalar(255, 255, 255));

        // 시간 표시
        size_t commaPos = currentTime.find(", ");
        if (commaPos != std::string::npos) {
            drawText(ft2, dashboard, currentTime.substr(0, commaPos), cv::Point(10, 120), 10, cv::Scalar(255, 255, 255));
            drawText(ft2, dashboard, currentTime.substr(commaPos + 2), cv::Point(10, 140), 20, cv::Scalar(255, 255, 255));
        }

        // 룸 컨트롤
        cv::rectangle(dashboard, cv::Point(200, 10), cv::Point(290, 130), cv::Scalar(255, 255, 255), 1);
        drawText(ft2, dashboard, "Room", cv::Point(215, 30), 20, cv::Scalar(255, 255, 255));
        drawButton(dashboard, "ON", cv::Point(210, 50), cv::Point(240, 80), cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), ft2);
        drawButton(dashboard, "OFF", cv::Point(250, 50), cv::Point(280, 80), cv::Scalar(255, 255, 255), cv::Scalar(0, 0, 0), ft2);
        drawText(ft2, dashboard, "25도", cv::Point(215, 110), 20, cv::Scalar(255, 255, 255));
        drawText(ft2, dashboard, "59.6%", cv::Point(215, 130), 20, cv::Scalar(255, 255, 255));

        // 뉴스 섹션
        cv::rectangle(dashboard, cv::Point(10, 500), cv::Point(290, 590), cv::Scalar(255, 255, 255), 1);
        drawText(ft2, dashboard, "오늘 주요 기사", cv::Point(20, 520), 20, cv::Scalar(255, 255, 255));

        // 이미지 표시
        cv::imshow("Dashboard", dashboard);

        // 1초 대기 및 'ESC' 키로 종료
        if (cv::waitKey(1000) == 27) {
            break;
        }
    }

    return 0;
}
