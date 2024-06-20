#ifndef SMARTMIRROR_H
#define SMARTMIRROR_H
#include <iostream>
#include <vector>

#define OFF 0
#define ON 1

int power;
int display = 0;
std:: vector <char> weather;
std:: vector <char> olmpic;
std:: vector <char> sports;
std:: vector <double> room_condition;
std:: vector <int> room_light;

// smartmirror
void runningSmartMirror(int user, int display=2, std::vector <char> weather, std::vector <char> olmpic, std::vector <char> sports, std::vector <double> room_condition, std::vector <int> room_light);
void waittingSmartMirror(int display, std::vector <char> weather, std::vector <char> olmpic, std::vector <char> sports, std::vector <double> room_condition);
void setUser(int user_number);

// setupServerSocket
void setServerSocket();
void readServerSocket();
void writeServerSocket();
void closeServerSocket();

// get data
void getUserData();             // name, user_number, place, function
void getRoomLightData();
void getWeatherData();		    // thread 1hour
void getOlmpicData();		    // therad 1hour
void getSportsData();		    // thread 1hour
void getRoomCondition();		// thread 30sec, 1min


// display
void Display(int display);
void waitDisplay();
void menuDisplay();
void mainDisplay();

#endif