#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // 소켓 파일 디스크립터 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 주소 구조체 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓을 주소에 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 연결 요청 대기
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // 연결 수락
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted\n");

    // 실시간 명령 전송 루프
    while (1) {
        printf("Enter command (ON/OFF) or brightness value (0-255): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // 개행 문자 제거

        if (strcmp(buffer, "ON") == 0 || strcmp(buffer, "OFF") == 0 || 
            (atoi(buffer) >= 0 && atoi(buffer) <= 255)) {
            send(new_socket, buffer, strlen(buffer), 0);
            printf("Sent command: %s\n", buffer);
        } else {
            printf("Invalid input. Please enter 'ON', 'OFF', or a brightness value between 0 and 255.\n");
        }
    }

    // 소켓 종료
    close(new_socket);
    close(server_fd);
    return 0;
}