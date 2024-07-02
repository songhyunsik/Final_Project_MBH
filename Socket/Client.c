// gcc -o client Client.c -lws2811 -lm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "ws2811.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define LED_PIN 18
#define LED_COUNT 1
#define TARGET_FREQ WS2811_TARGET_FREQ
#define GPIO_PIN 4
#define DMA 10

ws2811_t ledstring = {
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel = {
        [0] = {
            .gpionum = GPIO_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = 255,
            .strip_type = WS2811_STRIP_GRB,
        },
    },
};

// LED 초기화 함수
void initialize_led() {
    if (ws2811_init(&ledstring)) {
        fprintf(stderr, "ws2811_init failed\n");
        exit(1);
    }
}

// LED 해제 함수
void finalize_led() {
    ws2811_fini(&ledstring);
}

// LED 상태 설정 함수
void set_sk6812_state(const char* command) {
    if (strcmp(command, "ON") == 0) {
        printf("SK6812 LED is ON\n");
        ledstring.channel[0].leds[0] = 0x00FF00; // Green color
        ws2811_render(&ledstring);
    } else if (strcmp(command, "OFF") == 0) {
        printf("SK6812 LED is OFF\n");
        ledstring.channel[0].leds[0] = 0x000000; // Off
        ws2811_render(&ledstring);
    } else {
        int brightness = atoi(command);
        printf("Setting SK6812 LED brightness to %d\n", brightness);
        ledstring.channel[0].brightness = brightness;
        ws2811_render(&ledstring);
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    initialize_led();

    // 소켓 파일 디스크립터 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        finalize_led();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 주소 변환
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        finalize_led();
        return -1;
    }

    // 서버에 연결 요청
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        finalize_led();
        return -1;
    }

    printf("Connected to the server\n");

    // 실시간 명령 수신 루프
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            buffer[valread] = '\0';
            set_sk6812_state(buffer); // 명령에 따라 LED 제어
        }
    }

    // 소켓 종료
    close(sock);
    finalize_led();
    return 0;
}
