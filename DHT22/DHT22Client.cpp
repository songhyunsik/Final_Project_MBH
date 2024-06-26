#include <wiringPi.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

#define MAX_TIMINGS 85
#define DHT_PIN 7  // BCM GPIO 4
#define SERVER_IP "192.168.1.100"  // 서버 IP 주소로 변경
#define SERVER_PORT 8080

int data[5] = {0, 0, 0, 0, 0};

void readDHT22Data(float &humidity, float &temperature) {
    uint8_t lastState = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    pinMode(DHT_PIN, OUTPUT);
    digitalWrite(DHT_PIN, LOW);
    delay(18);
    digitalWrite(DHT_PIN, HIGH);
    delayMicroseconds(40);
    pinMode(DHT_PIN, INPUT);

    for (i = 0; i < MAX_TIMINGS; i++) {
        counter = 0;
        while (digitalRead(DHT_PIN) == lastState) {
            counter++;
            delayMicroseconds(1);
            if (counter == 255) break;
        }
        lastState = digitalRead(DHT_PIN);

        if (counter == 255) break;

        if ((i >= 4) && (i % 2 == 0)) {
            data[j / 8] <<= 1;
            if (counter > 16)
                data[j / 8] |= 1;
            j++;
        }
    }

    if ((j >= 40) &&
        (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
        humidity = (float)((data[0] << 8) + data[1]) / 10;
        if (humidity > 100) humidity = data[0];

        temperature = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
        if (temperature > 125) temperature = data[2];

        if (data[2] & 0x80) temperature = -temperature;
    } else {
        std::cerr << "Data not good, skip\n";
        humidity = temperature = -1;
    }
}

int main() {
    if (wiringPiSetup() == -1) {
        std::cerr << "wiringPi setup failed!\n";
        return -1;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed\n";
        return -1;
    }

    while (true) {
        float humidity, temperature;
        readDHT22Data(humidity, temperature);

        if (humidity != -1 && temperature != -1) {
            std::string data = "Humidity: " + std::to_string(humidity) + " Temperature: " + std::to_string(temperature);
            send(sock, data.c_str(), data.length(), 0);
        }

        sleep(1);
    }

    return 0;
}