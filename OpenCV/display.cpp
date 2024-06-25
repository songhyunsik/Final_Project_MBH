#include <iostream>
#include <chrono>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <curl/curl.h>
#include <json/json.h>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch (std::bad_alloc& e) {
        return 0;
    }
    return newLength;
}

std::string getNews() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        std::string apiKey = "NEWSAPI_KEY";
        std::string url = "https://newsapi.org/v2/top-headlines?country=kr&apiKey=" + apiKey;
        
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

std::string parseNews(const std::string& jsonData) {
    Json::Reader reader;
    Json::Value obj;
    reader.parse(jsonData, obj);

    if (!obj["articles"].isNull() && obj["articles"].size() > 0) {
        return obj["articles"][0]["title"].asString();
    }
    return "No news available";
}

int main() {
    cv::Mat display(900, 1600, CV_8UC3, cv::Scalar(255, 255, 255));

    while (true) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm* parts = std::localtime(&now_c);
        
        std::string newsData = getNews();
        std::string news = parseNews(newsData);

        display.setTo(cv::Scalar(255, 255, 255));

        char buffer[100];
        std::strftime(buffer, 100, "%Y-%m-%d %H:%M:%S", parts);
        cv::putText(display, buffer, cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);

        std::string weather = "Sunny 25C";
        cv::putText(display, weather, cv::Point(1200, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);

        cv::putText(display, "Latest News: " + news, cv::Point(10, 850), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);

        cv::imshow("Display", display);

        if (cv::waitKey(1000) >= 0) break;
    }

    return 0;
}
