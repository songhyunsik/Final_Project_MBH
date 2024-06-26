#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

std::string day_time_API;
std::string weather_API;
std::string news_API;

std::string day_time_data;
std::string weather_data;
std::string news_data;

std::string getApi(std::string){
	std::string data;	
	data = curl("www.temp_site");
	return data;
}


int main() {
    cv::Mat img = cv::Mat(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));

	// api 데이터 다운
	day_time_data = getApi(day_time_API);
	weather_data = getApi(weather_API);
	news_data = getApi(news_API);

	// 화면 텍스트 출력
    cv::putText(img,day_time_data, Point(50,50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255,255,255),2,Link_AA);
    cv::putText(img,weather_data, Point(200,50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255,255,255),2,Link_AA);
    cv::putText(img,news_data, Point(50,500), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255,255,255),2,Link_AA);

    cv::imshow("Smart_Mirror", img);
    cv::waitKey(0);

    return 0;
}