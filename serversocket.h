#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

class Server {
public:
    Server(int port);
    ~Server();
    void run();

private:
    static void* handleClient(void* socket_desc);
    static void sendToCloud(const std::string& data);
    int server_fd;
    int port;
    struct sockaddr_in address;
};

#endif // SERVER_H
