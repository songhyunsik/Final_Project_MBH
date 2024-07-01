/**
 * Read data (Temperature & Relative Humidity) from DHT22 sensor.
 * Data is transmitted bit by bit.  Start of bit is signaled by line going LOW,
 * then value of bit is determined by how long the line stays HIGH.
 * Based on code found at http://www.uugear.com/portfolio/read-dht1122-temperature-humidity-sensor-from-raspberry-pi/
 * Writen by Chris Wolcott.
 * Adapted by Q-engineering 7-6-2021
 */

#include <iostream>
#include <fstream>      // std::ofstream
#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "DHT22.h"

#define DHT_PIN     0                   // WiringPi 0 = BCM 17 = connector pin 11

int main( void ) {
    //socket init
    int sock = 0;
    struct sockaddr_in  serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    char buffer[50];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }
    
    //dht22 init
    int t=0;
    TDHT22 *MySensor = new TDHT22(DHT_PIN);
    MySensor->Init();                   

    // connect ip
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }


    while(1){
        MySensor->Fetch();
        std::cout << "Temp : " << MySensor->Temp  << " °C  Humidity : " << MySensor->Hum << " %" << std::endl;
        sprintf(buffer,"%f",MySensor->Temp);
        send(sock, buffer, strlen(buffer), 0);
        sprintf(buffer,"%f",MySensor->Hum);
        send(sock, buffer, strlen(buffer), 0);
        delay(1000);
        t++;
    }

    delete MySensor;
    close(sock);
    return(0);
}