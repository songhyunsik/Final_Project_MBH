// socket(client)1sec try connect server_socket
// socket(data): 1sec send data
// -lwiringPi
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
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


#define MAX_TIMINGS 85
#define DHT_PIN     0  /* BCM-11 */
#define SERVER_PORT 8080


int data[5] = { 0, 0, 0, 0, 0 };

typedef struct {
    float temperature;
    float humidity;
} DHTData;

void read_dht_data(DHTData *dhtData);
int connect2Server(const char *server_ip, int server_port);

int main(void)
{
    printf("Raspberry Pi DHT22 temperature and humidity test\n");

    // Initialize wiringPi
    if (wiringPiSetup() == -1) {
        printf("Failed to initialize wiringPi\n");
        exit(1);
    }

    int sockfd;
    while (1)
    {
        sockfd = connect2Server("192.168.0.71", SERVER_PORT);
        if (sockfd == -1) {
            printf("Retrying connection in 1 sec\n");
            sleep(1);
            continue;
        }

        while (1) {
            DHTData dhtData;
            read_dht_data(&dhtData);
            char buffer[50];

            if (dhtData.temperature != -1 && dhtData.humidity != -1) {
                snprintf(buffer, sizeof(buffer), "Humidity = %.1f %% Temperature = %.1f *C\n", dhtData.humidity, dhtData.temperature);
            } else {
                snprintf(buffer, sizeof(buffer), "Data not good, skip\n");
            }

            if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
                printf("Send failed, reconnecting...\n");
                break;
            }

            delay(1000); // wait 1 sec
        }


    }
        close(sockfd);
    return 0;
}

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

int connect2Server(const char *server_ip, int server_port) {
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation failed\n");
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port);
    servaddr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("Connection to server failed\n");
        close(sockfd);  // Make sure to close the socket if connection fails
        return -1;
    }

    return sockfd;
}
