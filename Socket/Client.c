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
            .brightness = 50,
            .strip_type = WS2811_STRIP_GRB,
        },
    },
};

// LED 초기화 함수
// void initialize_led() {
//     if (ws2811_init(&ledstring)) {
//         fprintf(stderr, "ws2811_init failed\n");
//         exit(1);
//     }
// }

// LED 해제 함수
// void finalize_led() {
//     ws2811_fini(&ledstring);
// }

// LED 상태 설정 함수
void set_sk6812_state(const char* command) {
    if (strcmp(command, "ON") == 0) {
        printf("SK6812 LED is ON\n");
        ledstring.channel[0].leds[0] = 0xFFFFFF; // Green color
        ws2811_render(&ledstring);
    } else if (strcmp(command, "OFF") == 0) {
        printf("SK6812 LED is OFF\n");
        ledstring.channel[0].leds[0] = 0x000000; // Off
        ws2811_render(&ledstring);
    } else {
        int brightness = atoi(command);
        printf("SK6812 LED의 밝기를 %d으로(로) 설정합니다.\n", brightness);
        ledstring.channel[0].brightness = brightness;
        ws2811_render(&ledstring);
    }
}

void send_brightness_to_server(int sock, const char *buffer) {
    send(sock, buffer, strlen(buffer), 0);
    printf("현재 밝기 데이터를 서버로 보냅니다. : %s\n", buffer);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    //initialize_led();

    // 소켓 파일 디스크립터 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("소켓 생성 에러.\n");
        //finalize_led();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 주소 변환
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("유효하지 않은 주소 / 지원되지 않는 주소\n");
        //finalize_led();
        return -1;
    }

    // 서버에 연결 요청
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("연결 실패.");
        //finalize_led();
        return -1;
    }

    printf("서버에 연결합니다!\n");

    // 실시간 명령 수신 루프
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            buffer[valread] = '\0';
            if (strcmp(buffer, "ON") == 0 || strcmp(buffer, "OFF") == 0) {
                printf("커맨드 커맨드를 받았습니다. : %s\n", buffer);
            }
            else {
                printf("서버로부터 데이터를 받아옵니다. : %s\n", buffer);  // 서버에서 받은 값 출력
            }
            //set_sk6812_state(buffer); // 명령에 따라 LED 제어
            send_brightness_to_server(sock, buffer); // 현재 밝기 값을 서버로 전송
        }
    }

    // 소켓 종료
    close(sock);
    //finalize_led();
    return 0;
}
