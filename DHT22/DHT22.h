#ifndef DHT22.H
#define DHT22.H

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_TIMINGS 85
#define DHT_PIN     0  /* BCM-11 */

int data[5] = { 0, 0, 0, 0, 0 };

typedef struct {
    float temperature;
    float humidity;
} DHTData;

void read_dht_data(DHTData *dhtData);


#endif