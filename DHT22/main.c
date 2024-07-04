// socket(client)1sec try connect server_socket
// socket(data): 1sec send data
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
#include <string>
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
    printf("Raspberry Pi DHT22 temperature_humidity test\n");
    // wiringPi 세팅 확인
	if (wiringPiSetup() == -1) exit(1);
	int sockfd;
    while (1)
    {
		// server_ip: 192.168.0.57
		if(sockfd = connect2Server("192.168.0.57",SERVER_PORT)){
			printf("Retrying connection in 1sec");
			sleep(1);
			continue;
		}
		while(1){
			// 온도, 습도 데이터 출력
			DHTData dhtData;
            read_dht_data(&dhtData);
            if (dhtData.temperature != -1 && dhtData.humidity != -1){
                char buffer[50];
                snprintf(buffer, sizeof(buffer), "Humidity = %.1f %% Temperature = %.1f *C\n", dhtData.humidity, dhtData.temperature);
                if (send(sockfd, buffer, strlen(buffer), 0) < 0){
                    printf("Send failed, reconnecting...\n");
                    break;
                }
				// for(int i = 0; i<49; ++i){
				// 	printf("%s",buffer[i]);
				// }
            }
            else{
                char *error_msg = "Data not good, skip\n";
                if (send(sockfd, error_msg, strlen(error_msg), 0) < 0){
                    printf("Send failed, reconnecting...\n");
                    break;
                }
            }
            delay(1000); // wait 1sec
        }
        }
		close(sockfd);
    }
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

    // 데이터 수집
    for (i = 0; i < MAX_TIMINGS; i++){
        counter = 0;
        while (digitalRead(DHT_PIN) == laststate){
            counter++;
            delayMicroseconds(1);
            if (counter == 255)	break; // 오류방지 타임아웃 매커니즘
        }
        laststate = digitalRead(DHT_PIN);

        if (counter == 255) break;
        
        // 데이터 해석
        if ((i >= 4) && (i % 2 == 0)){
            // Shove each bit into the storage bytes
            data[j / 8] <<= 1;
            if (counter > 16){
                data[j / 8] |= 1;
            }
            j++;
        }
    }

    // 데이터 모두 읽었는지 확인작업
    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))){
        dhtData->humidity = (float)((data[0] << 8) + data[1]) / 10; // 16바이트 습도
        dhtData->temperature = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10; // 16바이트 온도
		// 음수온도 처리 조건
        if (data[2] & 0x80){
            dhtData->temperature = -dhtData->temperature;
        }
    }
    else{
        dhtData->temperature = -1;
        dhtData->humidity = -1;     
    }
}

int connect2Server(const char *server_ip, int server_port){
	int sockfd;
	struct sockaddr_in servaddr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM,0) < 0)){
		printf("It's failed socket\n");
		return -1;
	}
	// 서버 구조체
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	servaddr.sin_addr.s_addr = inet_addr(server_ip);

	// 서버연결
	if(connect(sockfd,(struct sockaddr*)&servaddr, sizeof(servaddr))< 0){
		printf("Connection to server failed\n");
		return -1;
	}

	return sockfd;
}
