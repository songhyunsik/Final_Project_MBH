#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include "data.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8080
#define INTERVAL 30 // seconds
#define CONNECT_INTERVAL 1 // seconds


int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // data가져와서 char*로 변환
    DHT22Result data = read_dht22();
    char* humi = (char*)int2Char(data.humidity);
    char* temp = (char*)int2Char(data.temperature);
    

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return -1;
    }

    // 서버에 연결 시도 루프
    while (true) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
            break;
        } else {
            close(sockfd);
            std::this_thread::sleep_for(std::chrono::seconds(CONNECT_INTERVAL));
        }
    }
    // 데이터 송신
    while (true) {
        send(sockfd, humi, strlen(humi), 0);
        std::cout << "Message sent: " << humi << std::endl;
        send(sockfd, temp, strlen(temp), 0);
        std::cout << "Message sent: " << tmep << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(INTERVAL));
    }

    close(sockfd);
    return 0;
}