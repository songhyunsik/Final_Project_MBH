// 라즈베리파이 5 클라이언트 코드

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char data[BUFFER_SIZE] = {0};
    int brightness = 0;

    // 소켓 파일 설명자 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n 소켓 생성 에러 \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 주소 설정
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n 유효하지 않은 주소 / 지원되지 않는 주소 \n");
        return -1;
    }

    // 서버에 연결 요청
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n 연결 실패 \n");
        return -1;
    }

    // 1초마다 서버로부터 데이터 수신 및 조절할 밝기 값을 서버로 전송
    while (1) {
        printf("0에서 100 사이의 밝기 값을 입력하세요: ");
        scanf("%d", &brightness);
        printf("\n");

        // 밝기 값 전송
        snprintf(data, sizeof(data), "%d", brightness);
        send(sock, data, strlen(data), 0);

        // 서버로부터 데이터 수신
        memset(data, 0, BUFFER_SIZE); // 버퍼 초기화
        int valread = read(sock, data, BUFFER_SIZE);
        if (valread > 0) {
            printf("현재 LED의 밝기 데이터를 받았습니다...%s\n", data);
        }
        sleep(1); // 1초 대기
    }

    close(sock);
    return 0;
}