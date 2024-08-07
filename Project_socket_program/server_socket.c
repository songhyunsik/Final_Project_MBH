/* 
A: 서버소켓
기능 : 
	1. 데이터를 주고 받을 수 있는 소켓 4개
	2. 클라이언트와의 소켓연결이 끝나면 재 사용할 수 있음
	3. B클라이언트, C클라이언트에서 받은 데이터를 csv파일로 저장
		csv 헤더 >> year, month, day, hour, min, sec, data
	4. D클라이언트 소켓에서 요청할 때마다 csv파일을 전송

B: 클라이언트 소켓
기능:
	1. 1초마다 데이터를 생산하고("the testNumver is 1",the testNumver is 2",the testNumver is 3", ...) 데이터를 서버소켓에게 send()

C: 클라이언트 소켓
기능:
	1. 서버에서 데이터를 받으면 데이터에 따라서 변수가 변함(bool flag)
	2. 변수가 변할 때 마다 터미널 창에 출력((printf("LED is ON"))or(printf("LED is OFF"))
	3. 변한 변수를 다시 서버에게 전송

D: 클라이언트 소켓
기능:
	1. 서버에게 csv파일을 전송 요청

공통기능: 각 소켓은 연결된 후 끊어지지 않음. pthread를 사용한 소켓 동시 운영

주의사항: 각 클라이언트 소켓은 자신의 소켓이 무엇인지 확인하기 위한 데이터를 전송해야 한다.
파일 >> "/home/hrd/Desktop/data/"
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 4

int client_sockets[MAX_CLIENTS] = {0};
FILE *humi_temp_file;
FILE *led_file;
pthread_mutex_t file_mutex;

void write_csv_header(FILE *file, const char *header) {
    fprintf(file, "%s\n", header);
}

void write_csv_data(FILE *file, const char *data) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    pthread_mutex_lock(&file_mutex);
    fprintf(file, "%d,%d,%d,%d,%d,%d,%s\n", 
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, data);
    fflush(file);
    pthread_mutex_unlock(&file_mutex);
}

void *handle_client(void *arg) {
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int valread;

    // 클라이언트 식별
    valread = read(sock, buffer, BUFFER_SIZE);
    if (valread <= 0) {
        close(sock);
        return NULL;
    }
    buffer[valread] = '\0';

    if (strcmp(buffer, "B") == 0) {
        // 클라이언트 B 처리
        while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
            buffer[valread] = '\0';
            write_csv_data(humi_temp_file, buffer);
        }
    } else if (strcmp(buffer, "C") == 0) {
        // 클라이언트 C 처리
        while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
            buffer[valread] = '\0';
            write_csv_data(led_file, buffer);
            send(sock, buffer, strlen(buffer), 0);  // 상태를 다시 전송
        }
    } else if (strcmp(buffer, "D") == 0) {
        // 클라이언트 D 처리
        while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
            if (strcmp(buffer, "REQUEST_CSV") == 0) {
                FILE *csv_file_read = fopen("/home/hrd/Desktop/data/data.csv", "r");
                if (csv_file_read != NULL) {
                    while (fgets(buffer, BUFFER_SIZE, csv_file_read) != NULL) {
                        send(sock, buffer, strlen(buffer), 0);
                    }
                    fclose(csv_file_read);
                }
            }
        }
    }

    close(sock);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == sock) {
            client_sockets[i] = 0;
            break;
        }
    }

    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t tid[MAX_CLIENTS];

    pthread_mutex_init(&file_mutex, NULL);

    // CSV 파일 열기
    humi_temp_file = fopen("/home/hrd/Desktop/data/Humi_temp.csv", "w");
    if (humi_temp_file == NULL) {
        perror("Failed to open Humi_temp.csv");
        exit(EXIT_FAILURE);
    }
    write_csv_header(humi_temp_file, "year,month,day,hour,min,sec,data");

    led_file = fopen("/home/hrd/Desktop/data/LED.csv", "w");
    if (led_file == NULL) {
        perror("Failed to open LED.csv");
        exit(EXIT_FAILURE);
    }
    write_csv_header(led_file, "year,month,day,hour,min,sec,data");

    // 소켓 파일 기술자 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        fclose(humi_temp_file);
        fclose(led_file);
        exit(EXIT_FAILURE);
    }

    // 소켓 옵션 설정
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        fclose(humi_temp_file);
        fclose(led_file);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓에 주소 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        fclose(humi_temp_file);
        fclose(led_file);
        exit(EXIT_FAILURE);
    }

    // 연결 대기 상태 설정
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        fclose(humi_temp_file);
        fclose(led_file);
        exit(EXIT_FAILURE);
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = new_socket;
                pthread_create(&tid[i], NULL, handle_client, &new_socket);
                break;
            }
        }
    }

    close(server_fd);
    fclose(humi_temp_file);
    fclose(led_file);
    pthread_mutex_destroy(&file_mutex);

    return 0;
}
