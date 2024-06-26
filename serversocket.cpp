#include "Server.h"

Server::Server(int port) : port(port) {
    int opt = 1;
    int addrlen = sizeof(address);

    // 소켓 파일 설명자 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 소켓 옵션 설정
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // 소켓과 포트를 바인딩
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 연결 대기
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
}

Server::~Server() {
    close(server_fd);
}

void Server::run() {
    int new_socket1, new_socket2;
    int addrlen = sizeof(address);
    pthread_t thread_id1, thread_id2;

    // 첫 번째 클라이언트 연결 수락
    if ((new_socket1 = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    std::cout << "First client connected." << std::endl;

    // 두 번째 클라이언트 연결 수락
    if ((new_socket2 = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    std::cout << "Second client connected." << std::endl;

    // 첫 번째 클라이언트 처리 스레드 생성
    if (pthread_create(&thread_id1, NULL, handleClient, (void*)&new_socket1) < 0) {
        perror("could not create thread");
        exit(EXIT_FAILURE);
    }

    // 두 번째 클라이언트 처리 스레드 생성
    if (pthread_create(&thread_id2, NULL, handleClient, (void*)&new_socket2) < 0) {
        perror("could not create thread");
        exit(EXIT_FAILURE);
    }

    // 스레드 종료 대기
    pthread_join(thread_id1, NULL);
    pthread_join(thread_id2, NULL);
}

void* Server::handleClient(void* socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[1024];
    int valread;

    // 클라이언트로부터 데이터 읽기
    while ((valread = read(sock, buffer, sizeof(buffer))) > 0) {
        buffer[valread] = '\0';

        // 터미널에 데이터 출력
        std::cout << "Received data: " << buffer << std::endl;

        // 클라우드로 데이터 전송
        sendToCloud(buffer);
    }

    if (valread == 0) {
        std::cout << "Client disconnected." << std::endl;
    } else {
        perror("recv failed");
    }

    close(sock);
    return 0;
}

void Server::sendToCloud(const std::string& data) {
    // 클라우드에 데이터를 전송하는 코드 (예: HTTP POST 요청)
    // 여기서는 간단하게 HTTP 요청을 printf로 표시합니다.
    std::cout << "Sending data to cloud: " << data << std::endl;
}
