#include <wiringPi.h>  
#include <iostream>    
#include <cstdlib>     
#include <cstdint>     
#include "data.h"

#define MAX_TIMINGS 85 // 최대 시간 측정 수 설정
#define DHT_PIN 4      // DHT 데이터 핀 설정 (GPIO 4)

int data[5] = {0, 0, 0, 0, 0};

DHT22Result read_dht22() {
    uint8_t laststate = HIGH; // 마지막 상태 변수 초기화
    uint8_t counter = 0;      // 카운터 변수 초기화
    uint8_t j = 0, i;         // 루프 카운터 변수 초기화

    data[0] = data[1] = data[2] = data[3] = data[4] = 0; // 데이터 배열 초기화

    pinMode(DHT_PIN, OUTPUT); // DHT 핀을 출력 모드로 설정
    digitalWrite(DHT_PIN, LOW); // 핀을 LOW로 설정 (시작 신호)
    delay(18); // 18ms 동안 대기
    digitalWrite(DHT_PIN, HIGH); // 핀을 HIGH로 설정
    delayMicroseconds(40); // 40 마이크로초 동안 대기
    pinMode(DHT_PIN, INPUT); // DHT 핀을 입력 모드로 설정

    for (i = 0; i < MAX_TIMINGS; i++) { // 최대 시간 측정 수 만큼 반복
        counter = 0;
        while (digitalRead(DHT_PIN) == laststate) { // 핀 상태가 변할 때까지 카운트
            counter++;
            delayMicroseconds(1);
            if (counter == 255) { // 카운터가 최대에 도달하면 루프 종료
                break;
            }
        }
        laststate = digitalRead(DHT_PIN); // 현재 핀 상태 저장

        if (counter == 255) break;

        if ((i >= 4) && (i % 2 == 0)) { // 처음 3개의 변화를 무시하고 2번마다 데이터 수집
            data[j / 8] <<= 1;
            if (counter > 16) { // 16 마이크로초 이상이면 1로 설정
                data[j / 8] |= 1;
            }
            j++;
        }
    }

    DHT22Result result;
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
    return buffer;
}