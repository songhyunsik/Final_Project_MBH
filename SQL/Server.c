#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <mysql/mysql.h>

#define PORT 8080 // 포트 번호 설정
#define BUFFER_SIZE 1024 // 버퍼 크기 설정
#define MAX_CLIENTS 4 // 최대 클라이언트 수 설정

int client_sockets[MAX_CLIENTS] = {0}; // 클라이언트 소켓 배열 초기화
FILE *humi_temp_file; // 온습도 데이터 파일 포인터
FILE *led_file; // LED 데이터 파일 포인터
pthread_mutex_t file_mutex; // 파일 접근을 위한 뮤텍스

// MySQL 연결 정보 설정
#define DB_HOST "localhost"
#define DB_USER "HRD"
#define DB_PASS "1111"
#define DB_NAME "smart_mirror"

// MySQL 연결 객체
MYSQL *conn;

// CSV 파일에 헤더 쓰기 함수
void write_csv_header(FILE *file, const char *header) {
    fprintf(file, "%s\n", header);
}

// CSV 파일에 데이터 쓰기 함수
void write_csv_data(FILE *file, const char *data) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    pthread_mutex_lock(&file_mutex); // 뮤텍스 잠금
    fprintf(file, "%d,%d,%d,%d,%d,%d,%s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, data);
    fflush(file);
    pthread_mutex_unlock(&file_mutex); // 뮤텍스 해제
}

// MySQL 데이터베이스에 데이터 쓰기 함수
void write_db_data(const char *table, const char *data) {
    char query[512];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(query, sizeof(query),
             "INSERT INTO %s (year, month, day, hour, minute, second, data) VALUES (%d, %d, %d, %d, %d, %d, '%s')",
             table, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec, data);

    if (mysql_query(conn, query)) { // 쿼리 실행
        fprintf(stderr, "INSERT error: %s\n", mysql_error(conn));
    }
}

// 클라이언트 요청을 처리하는 함수
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
            write_csv_data(humi_temp_file, buffer); // CSV 파일에 쓰기
            write_db_data("humi_temp", buffer); // MySQL 데이터베이스에 쓰기
        }
    } else if (strcmp(buffer, "C") == 0) {
        // 클라이언트 C 처리
        while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
            buffer[valread] = '\0';
            write_csv_data(led_file, buffer); // CSV 파일에 쓰기
            write_db_data("led", buffer); // MySQL 데이터베이스에 쓰기
            send(sock, buffer, strlen(buffer), 0); // 상태를 다시 전송
        }
    } else if (strcmp(buffer, "D") == 0) {
        // 클라이언트 D 처리
        while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
            if (strcmp(buffer, "REQUEST_CSV") == 0) {
                // CSV 파일 요청 처리
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

    close(sock); // 클라이언트 소켓 닫기
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

    pthread_mutex_init(&file_mutex, NULL); // 뮤텍스 초기화

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

    // MySQL 연결 초기화
    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return EXIT_FAILURE;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 소켓 파일 디스크립터 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        fclose(humi_temp_file);
        fclose(led_file);
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    // 소켓 옵션 설정
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        fclose(humi_temp_file);
        fclose(led_file);
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스에서 연결 허용
    address.sin_port = htons(PORT); // 지정된 포트 사용

    // 소켓에 주소 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        fclose(humi_temp_file);
        fclose(led_file);
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    // 연결 대기 상태 설정
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        fclose(humi_temp_file);
        fclose(led_file);
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    // 클라이언트 연결 수락 및 처리
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

    // 리소스 정리
    close(server_fd);
    fclose(humi_temp_file);
    fclose(led_file);
    mysql_close(conn);
    pthread_mutex_destroy(&file_mutex);

    return 0;
}
