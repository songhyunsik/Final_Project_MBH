#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int new_socket) {
    char buffer[BUFFER_SIZE] = {0};

    while (1) {
        printf("밝기 조절 (0~100) / 'ON' 또는 'OFF'를 입력해주세요. : ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "ON") == 0 || strcmp(buffer, "OFF") == 0 || (0 <= atoi(buffer) && atoi(buffer) <= 100)) {
            send(new_socket, buffer, strlen(buffer), 0);
            printf("클라이언트로 데이터/커맨드를 전송합니다. : %s\n", buffer);
        } else {
            printf("'ON', 'OFF'을 입력해주시거나, 0~100 사이의 밝기값을 입력해주세요.\n");
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (valread > 0) {
            buffer[valread] = '\0';
            printf("현재 LED의 밝기 값: %s\n", buffer);
        } else {
            break; // 클라이언트 연결 종료 시 루프 탈출
        }
    }
    close(new_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 소켓 파일 디스크립터 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("소켓 생성 실패.");
        exit(EXIT_FAILURE);
    }

    // 주소 구조체 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓을 주소에 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("바인딩 실패.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 연결 요청 대기
    if (listen(server_fd, 3) < 0) {
        perror("요청 실패.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("%d 포트 연결 대기중입니다...\n", PORT);

    // 연결 수락 루프
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("승인 실패.");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("연결에 성공하였습니다!\n");
        handle_client(new_socket);
    }

    // 소켓 종료
    close(server_fd);
    return 0;
}
