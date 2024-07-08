#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080 // 서버와 동일한 포트 번호 사용
#define BUFFER_SIZE 1024 // 버퍼 크기 설정

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    // 가상의 온습도 데이터 (실제로는 센서 데이터 받아와야하는데 어떻게 하는지 모르겠음...)
    float temperature = 23.5;
    float humidity = 60.2;
    snprintf(buffer, BUFFER_SIZE, "Temperature: %.2f, Humidity: %.2f", temperature, humidity);

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 IP 주소 설정 (서버의 IP 주소로 변경)
    if (inet_pton(AF_INET, "163.152.224.133", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // 서버로 온습도 데이터 전송
    send(sock, buffer, strlen(buffer), 0);
    printf("Data sent: %s\n", buffer);

    // 서버로부터 응답 읽기 (필요한 경우)
    int valread = read(sock, buffer, BUFFER_SIZE);
    printf("Server response: %s\n", buffer);

    close(sock); // 소켓 닫기
    return 0;
}
