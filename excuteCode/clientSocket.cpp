#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main() {
    struct sockaddr_rc addr = { 0 };
    int sock;
    char dest[18] = "XX:XX:XX:XX:XX:XX";  // 서버 블루투스 주소로 변경

    // 블루투스 소켓 생성
    sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    addr.rc_family = AF_BLUETOOTH;
    str2ba(dest, &addr.rc_bdaddr);
    addr.rc_channel = (uint8_t) 1;

    // 서버에 연결 요청
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // 서버로 메시지 전송
    const char *message = "Hello from client";
    if (write(sock, message, strlen(message)) < 0) {
        perror("write");
        close(sock);
        exit(EXIT_FAILURE);
    }

    std::cout << "Message sent" << std::endl;

    // 소켓 닫기
    close(sock);

    return 0;
}