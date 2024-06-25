// 수신기 코드

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::udp;

int main() {
    boost::asio::io_service io_service;
    udp::socket socket(io_service, udp::endpoint(udp::v4(), 5005));

    std::cout << "Listening on port 5005..." << std::endl;

    while (true) {
        boost::array<char, 128> recv_buf;
        udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
        std::cout << "Received data: " << std::string(recv_buf.data(), len) << " from " << sender_endpoint << std::endl;
    }

    return 0;
}
