#ifndef SMARTMIRROR_H
#define SMARTMIRROR_H
#define OFF 0
#define ON 1


class SmartMirror
{	
public:
    int power;

	SmartMirror();
	void initSmartMirror();
	void runningSmartMirror();
    void options();
	~SmartMirror();
}; 

class User:public SmartMirror{
private:
    char name;
    int number;
    int function;
public:
    User();
    void selectUser();
};

class ServerSocket: public SmartMirror{
private:
    int server_fd;
    int new_socket;
    int opt;
    int addrlen;
    char buffer;
public:
    void createServerSocket();
    void setupServerSocket();
    void bindServerSocket();
    void listenServerSocket();
    void acceptServerSocket();
    void readServerSocket();
    void closeServerSocket();
};

class Display: public SmartMirror{
private:
    int size;
	int name;
public:
	Display();
	void displayData();
};

class Olmpic: public Display{
private:
    int country;
	int year;
    int sport_event;
public:
    Olmpic();
    void olmpicAnalysis();
};

class Weather: public Display{
private:
    int region;
public:
    Weather();
    void findWeather();
};

#endif