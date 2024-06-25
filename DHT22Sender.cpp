// 송신기 코드

#include <wiringPi.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <unistd.h>

#define DHT_PIN 7  // BCM_GPIO 4

using boost::asio::ip::udp;

class DHT22 {
public:
    DHT22(int pin) : pin(pin) {
        wiringPiSetup();
        pinMode(pin, OUTPUT);
    }

    bool readData(float &temperature, float &humidity) {
        uint8_t data[5] = {0, 0, 0, 0, 0};
        uint8_t laststate = HIGH;
        uint8_t counter = 0;
        uint8_t j = 0, i;

        // Send start signal
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        usleep(18000);  // 18ms
        digitalWrite(pin, HIGH);
        usleep(40);     // 40us
        pinMode(pin, INPUT);

        // Read data
        for (i = 0; i < 85; i++) {
            counter = 0;
            while (digitalRead(pin) == laststate) {
                counter++;
                delayMicroseconds(1);
                if (counter == 255) break;
            }
            laststate = digitalRead(pin);
            if (counter == 255) break;
            if ((i >= 4) && (i % 2 == 0)) {
                data[j / 8] <<= 1;
                if (counter > 16)
                    data[j / 8] |= 1;
                j++;
            }
        }

        // Checksum and data processing
        if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
            float h = (data[0] * 256 + data[1]) / 10.0;
            if (h > 100) h = data[0];  // For DHT22
            float c = (data[2] & 0x7F) * 256 + data[3];
            if (c > 125) c = data[2];  // For DHT22
            if (data[2] & 0x80) c = -c;
            c /= 10.0;
            humidity = h;
            temperature = c;
            return true;
        }
        return false;
    }

private:
    int pin;
};

int main() {
    DHT22 dht(DHT_PIN);

    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint;

    socket.open(udp::v4());

    remote_endpoint = udp::endpoint(boost::asio::ip::address::from_string("RECEIVER_IP_ADDRESS"), 5005);

    while (true) {
        float temperature, humidity;
        if (dht.readData(temperature, humidity)) {
            std::ostringstream oss;
            oss << "Temperature: " << temperature << " C, Humidity: " << humidity << " %";
            std::string message = oss.str();
            socket.send_to(boost::asio::buffer(message), remote_endpoint);
            std::cout << "Sent data: " << message << std::endl;
        } else {
            std::cerr << "Failed to get reading. Try again!" << std::endl;
        }
        sleep(2);  // 2 seconds delay
    }

    return 0;
}
