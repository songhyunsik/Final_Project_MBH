#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <curl/curl.h>

// API URL 정의
std::string day_time_API = "https://worldtimeapi.org/api/timezone/Asia/Seoul"; // 예시 시간 API
std::string weather_API = "https://api.openweathermap.org/data/2.5/weather?q=Seoul&appid=YOUR_API_KEY"; // 예시 날씨 API
std::string news_API = "https://newsapi.org/v2/top-headlines?country=kr&apiKey=YOUR_NEWS_API_KEY"; // 예시 뉴스 API

// 데이터 저장 변수
std::string day_time_data;
std::string weather_data;
std::string news_data;

// CURL 콜백 함수
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch (std::bad_alloc& e) {
        return 0;
    }
    return newLength;
}

// API 데이터 가져오기 함수
std::string getApi(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return readBuffer;
}

int main() {
    // 검은색 화면
    cv::Mat img = cv::Mat(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));

    // API 데이터 가져오기
    day_time_data = getApi(day_time_API);
    weather_data = getApi(weather_API);
    news_data = getApi(news_API);

    // 텍스트 위치 설정 및 출력
    cv::putText(img, day_time_data, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
    cv::putText(img, weather_data, cv::Point(200, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
    cv::putText(img, news_data, cv::Point(50, 400), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);

    // 화면 출력
    cv::imshow("Smart_Mirror", img);
    cv::waitKey(0);

    return 0;
}
