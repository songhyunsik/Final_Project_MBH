#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main() {
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = {0};
    int server_sock, client_sock, bytes_read, bytes_write;
    socklen_t opt = sizeof(rem_addr);

    // 블루투스 소켓 생성
    server_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (server_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 로컬 블루투스 주소 설정
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;

    // 소켓과 주소 바인딩
    if (bind(server_sock, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) < 0) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 클라이언트 연결 대기
    if (listen(server_sock, 1) < 0) {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    std::cout << "Waiting for a connection..." << std::endl;

    // 클라이언트 연결 허용
    client_sock = accept(server_sock, (struct sockaddr *)&rem_addr, &opt);
    if (client_sock < 0) {
        perror("accept");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 클라이언트 주소 출력
    char client_addr[18];
    ba2str(&rem_addr.rc_bdaddr, client_addr);
    std::cout << "Accepted connection from " << client_addr << std::endl;

    // 클라이언트로부터 메시지 읽기
    memset(buf, 0, sizeof(buf));
    bytes_read = read(client_sock, buf, sizeof(buf));
    if (bytes_read > 0) {
        std::cout << "Received message: " << buf << std::endl;
    }

    // 클라이언트로부터 메시지 전송
    memset(buf, 0, sizeof(buf));
    std::cout << "Enter message: ";
    std::cin.getline(buf, sizeof(buf));
    bytes_write = write(client_sock, buf, sizeof(buf));

    // 소켓 닫기
    close(client_sock);
    close(server_sock);

    return 0;
}