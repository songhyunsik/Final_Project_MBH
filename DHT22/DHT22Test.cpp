#include <wiringPi.h>  
#include <iostream>    
#include <cstdlib>     
#include <cstdint>     
#include "DHTData.h"
#define MAX_TIMINGS 85
#define DHT_PIN 4  // BCM GPIO 4

int data[5] = {0, 0, 0, 0, 0};

readDHT22Data read_dht22() {
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
            if (counter > 16) {
                data[j / 8] |= 1;
            }
            j++;
        }
    }

    readDHT22Data result;
     if ((j >= 40) && // 모든 데이터가 수집되었는지 확인
        (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) { // 체크섬 확인
        float h = (float)((data[0] << 8) + data[1]) / 10; // 습도 계산
        if (h > 100) {
            h = data[0];
        }
        float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10; // 섭씨 온도 계산
        if (c > 125) {
            c = data[2];
        }
        if (data[2] & 0x80) {
            c = -c;
        }
        float f = c * 1.8f + 32; // 화씨 온도로 변환
        result.humidity = static_cast<int>(h);
        result.temperatureC = static_cast<int>(c);
        result.temperatureF = static_cast<int>(f);
    } else {
        std::cerr << "데이터 읽기 실패" << std::endl; // 데이터 읽기 실패 메시지 출력
        result.humidity = -1;
        result.temperatureC = -1;
        result.temperatureF = -1;
    }
    return result;
}

char int2Char(int result) {
    char buffer[10];
    sprintf(buffer, "%d", result);
    return buffer[10];
}