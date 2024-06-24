#include <iostream>
#include <cstring>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
    struct sockaddr_rc addr = { 0 };
    int s, status;
    char dest[18] = "DB:3A:DD:1D:BA:F7"; // 서버의 MAC 주소로 변경

    // Create socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    str2ba(dest, &addr.rc_bdaddr);
    addr.rc_channel = (uint8_t) 1;

    // Connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    // Check if the connection was successful
    if(status == 0) {
        char buf[1024] = { 0 };
        std::string message;

        while(true) {
            std::cout << "Enter message: ";
            std::getline(std::cin, message);
            if(message == "exit") break;

            // Send a message
            status = write(s, message.c_str(), message.size());
            if(status < 0) {
                std::cerr << "Failed to send message" << std::endl;
                break;
            }

            // Receive a reply
            status = read(s, buf, sizeof(buf));
            if(status > 0) {
                std::cout << "Received: " << buf << std::endl;
            } else {
                std::cerr << "Failed to receive message" << std::endl;
                break;
            }
            memset(buf, 0, sizeof(buf));
        }
    } else {
        std::cerr << "Failed to connect" << std::endl;
    }

    // Close connection
    close(s);
    return 0;
}
