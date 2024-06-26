#ifndef DHTDATA_H
#define DHTDATA_H

struct readDHT22Data {
    int humidity;
    int temperature;
    int temperatureC;
    int temperatureF;
};

readDHT22Data read_dht22();
char int2Char(int result);
char float2Char(float result);

#endif //DHTDATA_H   