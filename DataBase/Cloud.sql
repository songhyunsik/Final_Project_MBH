#include <mysql/mysql.h>
#include <iostream>
#include <fstream>v 
#include <sstream>
#include <string>
#include <cstdlib>
#include <MQTTClient.h>

#define ADDRESS     "tcp://broker.hivemq.com:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "sql/query"
#define QOS         1
#define TIMEOUT     10000L

void finish_with_error(MYSQL *con) {
    std::cerr << mysql_error(con) << std::endl;
    mysql_close(con);
    exit(1);
}

std::string read_sql_file(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void execute_query(const std::string& query) {
    MYSQL *con = mysql_init(NULL);

    if (con == NULL) {
        std::cerr << "mysql_init() failed" << std::endl;
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "your_username", "your_password",
                           "your_database", 0, NULL, 0) == NULL) {
        finish_with_error(con);
    }

    if (mysql_query(con, query.c_str())) {
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);
    if (result == NULL) {
        finish_with_error(con);
    }

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        for(int i = 0; i < num_fields; i++) {
            std::cout << (row[i] ? row[i] : "NULL") << " ";
        }
        std::cout << std::endl;
    }

    mysql_free_result(result);
    mysql_close(con);
}

void delivered(void *context, MQTTClient_deliveryToken dt) {
    std::cout << "Message with token value " << dt << " delivery confirmed" << std::endl;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    std::cout << "Message arrived" << std::endl;
    std::cout << "     topic: " << topicName << std::endl;
    std::cout << "   message: " << std::string((char*)message->payload, message->payloadlen) << std::endl;

    execute_query(std::string((char*)message->payload, message->payloadlen));

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause) {
    std::cout << "\nConnection lost" << std::endl;
    std::cout << "     cause: " << cause << std::endl;
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <sql_file>" << std::endl;
        return 1;
    }

    std::string query = read_sql_file(argv[1]);

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        std::cout << "Failed to connect, return code " << rc << std::endl;
        exit(EXIT_FAILURE);
    }

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = (void*)query.c_str();
    pubmsg.payloadlen = query.length();
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;

    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    std::cout << "Waiting for up to " << (TIMEOUT/1000) << " seconds for publication of " << query
              << "\non topic " << TOPIC << " for client with ClientID: " << CLIENTID << std::endl;
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    std::cout << "Message with delivery token " << token << " delivered" << std::endl;

    MQTTClient_subscribe(client, TOPIC, QOS);

    std::cout << "Press Q<Enter> to quit" << std::endl;
    do {
        char c = getchar();
        if(c == 'Q' || c == 'q') break;
    } while(1);

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}