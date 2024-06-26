#ifndef DATA.H
#define DATA.H

struct DHT22Result {
    int humidity;
    int temperature;
};

DHT22Result read_dht22();
char int2Char(int result);
char float2Char(float result);

#endif DATA.H   