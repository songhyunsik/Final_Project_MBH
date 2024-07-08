/*
 *  dht.c:
 *	read temperature and humidity from DHT11 or DHT22 sensor
 *  Based on code found http://www.uugear.com/portfolio/read-dht1122-temperature-humidity-sensor-from-raspberry-pi/
 */


#include "DHT22.h"

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

    for (i = 0; i < MAX_TIMINGS; i++) {
        counter = 0;
        while (digitalRead(DHT_PIN) == laststate) {
            counter++;
            delayMicroseconds(1);
            if (counter == 255) break; // Timeout condition
        }
        laststate = digitalRead(DHT_PIN);

        if (counter == 255) break;

        if ((i >= 4) && (i % 2 == 0)) {
            data[j / 8] <<= 1;
            if (counter > 16) {
                data[j / 8] |= 1;
            }
            j++;
        }
    }

    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
        dhtData->humidity = (float)((data[0] << 8) + data[1]) / 10;
        dhtData->temperature = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
        if (data[2] & 0x80) {
            dhtData->temperature = -dhtData->temperature;
        }
    } else {
        dhtData->temperature = -1;
        dhtData->humidity = -1;     
    }
}