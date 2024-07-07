#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345 // 서버와 동일한 포트 번호 사용
#define BUFFER_SIZE 1024 // 버퍼 크기 설정

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[BUFFER_SIZE] = {0};

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 IP 주소 설정 (서버의 IP 주소로 변경)
    if (inet_pton(AF_INET, "your_server_ip", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // 서버로 메시지 전송
    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    // 서버로부터 메시지 읽기
    int valread = read(sock, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);

    close(sock); // 소켓 닫기
    return 0;
}
