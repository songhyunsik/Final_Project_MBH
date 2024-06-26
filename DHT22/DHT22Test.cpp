#include <wiringPi.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define MAX_TIMINGS 85
#define DHT_PIN 7  // BCM GPIO 4

int data[5] = {0, 0, 0, 0, 0};

void readDHT22Data() {
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
        float h = (float)((data[0] << 8) + data[1]) / 10;
        if (h > 100) h = data[0];

        float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
        if (c > 125) c = data[2];

        if (data[2] & 0x80) c = -c;

        float f = c * 1.8f + 32;

        std::cout << "Humidity = " << h << " %\n";
        std::cout << "Temperature = " << c << " *C (" << f << " *F)\n";

        std::ofstream outFile("dht22_data.txt");
        if (outFile.is_open()) {
            outFile << h << "\n" << c << "\n";
            outFile.close();
        }
    } else {
        std::cout << "Data not good, skip\n";
    }
}

int main() {
    if (wiringPiSetup() == -1) {
        std::cout << "wiringPi setup failed!\n";
        return -1;
    }

    while (true) {
        readDHT22Data();
        delay(1000);  // wait 1 second
    }

    return 0;
}