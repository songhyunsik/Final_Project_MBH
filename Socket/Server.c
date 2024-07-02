// 라즈베리파이 제로2 W 서버 코드
// gcc -o server Server.c -lws2811 -lm


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdint.h>
#include "ws2811.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define LED_COUNT 1

#define TARGET_FREQ WS2811_TARGET_FREQ
#define GPIO_PIN 18
#define DMA 10
#define STRIP_TYPE WS2811_STRIP_GRB

ws2811_t ledstring = {
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel = {
        [0] = {
            .gpionum = GPIO_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = 255,
            .strip_type = STRIP_TYPE,
        },
    },
};

// void SendToGoogleC(const char* project_id, const char* topic_id, const char* message) {
//     char command[BUFFER_SIZE];
//     snprintf(command, sizeof(command), "python3 send_to_pubsub.py %s %s \"%s\"", project_id, topic_id, message);
//     system(command);
// }

void adjust_led_brightness(int brightness) {
    // LED 밝기 조절 코드 추가
    ws2811_led_t color = (brightness << 16) | (brightness << 8) | brightness;
    ledstring.channel[0].leds[0] = color;
    ws2811_render(&ledstring);
    printf("LED 밝기를 %d로 조절합니다.\n", brightness);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char data[BUFFER_SIZE] = {0};
    int brightness = 0;

    // 구글 클라우드 프로젝트 ID 및 토픽 ID 설정
    // const char* project_id = "your_project_id";
    // const char* topic_id = "your_topic_id";

    // 소켓 파일 설명자 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("소켓 실패");
        exit(EXIT_FAILURE);
    }

    // 주소 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓을 해당 포트에 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("바인딩 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 연결 대기
    if (listen(server_fd, 3) < 0) {
        perror("연결 거부");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("%d포트 연결 대기중입니다...\n", PORT);

    // LED 초기화
    if (ws2811_init(&ledstring)) {
        fprintf(stderr, "ws2811_init failed\n");
        return 1;
    }

    // 연결 수락
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("연결 실패");
        close(server_fd);
        ws2811_fini(&ledstring);
        exit(EXIT_FAILURE);
    }

    // 1초마다 밝기 값 전송 및 클라이언트로부터 밝기 값 수신
    while (1) {
        // 클라이언트로부터 밝기 값 수신
        memset(data, 0, BUFFER_SIZE);
        int valread = read(new_socket, data, BUFFER_SIZE);
        if (valread > 0) {
            brightness = atoi(data); // 문자열을 정수로 변환
            if (brightness >= 0 && brightness <= 100) {
                adjust_led_brightness(brightness);
            }
        }

        // 현재 밝기 값 전송
        brightness = (brightness + 10) % 100; // 밝기 값 업데이트 (예시로 0에서 90까지 10씩 증가)
        snprintf(data, sizeof(data), "Brightness: %d", brightness);
        send(new_socket, data, strlen(data), 0);
        printf("밝기 데이터를 전송합니다...%s\n", data);
  
        // SendToGoogleC(project_id, topic_id, data);

        sleep(1); // 1초 대기
    }

    close(new_socket);
    close(server_fd);
    ws2811_fini(&ledstring);
    return 0;
}