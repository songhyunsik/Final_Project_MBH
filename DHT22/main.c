// socket(client)
// wiringPi
// raspberryPi zero 2w

/*
 *  dht.c:
 *	read temperature and humidity from DHT11 or DHT22 sensor
 *  Based on code found http://www.uugear.com/portfolio/read-dht1122-temperature-humidity-sensor-from-raspberry-pi/
 */

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_TIMINGS 85
#define DHT_PIN     0  /* GPIO-11 */

int data[5] = { 0, 0, 0, 0, 0 };

typedef struct {
    float temperature;
    float humidity;
} DHTData;

void read_dht_data(DHTData *dhtData)
{
    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    // Pull pin down for 18 milliseconds
    pinMode(DHT_PIN, OUTPUT);
    digitalWrite(DHT_PIN, LOW);
    delay(18);

    // Prepare to read the pin
    pinMode(DHT_PIN, INPUT);

    // 데이터 수집
    for (i = 0; i < MAX_TIMINGS; i++)
    {
        counter = 0;
        while (digitalRead(DHT_PIN) == laststate)	// 변화 감지
        {
            counter++;
            delayMicroseconds(1);
            if (counter == 255)			// 오류방지 타임아웃 메커니즘
            {
                break;
            }
        }
        laststate = digitalRead(DHT_PIN);

        if (counter == 255)
        {
            break;
        }

        // 데이터 해석
        if ((i >= 4) && (i % 2 == 0))
        {
            // Shove each bit into the storage bytes
            data[j / 8] <<= 1;
            if (counter > 16)
            {
                data[j / 8] |= 1;
            }
            j++;
        }
    }

    // 데이터 모두 읽었는지 확인작업
    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
    {
        dhtData->humidity = (float)((data[0] << 8) + data[1]) / 10; // 16바이트 습도
        dhtData->temperature = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10; // 16바이트 온도
		// 음수온도 처리 조건
        if (data[2] & 0x80)
        {
            dhtData->temperature = -dhtData->temperature;
        }
    }
    else // 데이터 확인 하지 아니할 때
    {
        dhtData->temperature = -1;
        dhtData->humidity = -1;     
    }
}

int main(void)
{
    printf("Raspberry Pi DHT22 temperature_humidity test\n");
    // wiringPi 세팅 확인
	if (wiringPiSetup() == -1) exit(1);

	
    while (1)
    {
		// 온도, 습도 데이터 출력
        DHTData dhtData;
        read_dht_data(&dhtData);
		if (dhtData.temperature != -1 && dhtData.humidity != -1)
        {
            printf("Humidity = %.1f %% Temperature = %.1f *C\n", dhtData.humidity, dhtData.temperature);
        }
        else printf("Data not good, skip\n");

        delay(1000); // 1초 대기
    }

    return 0;
}