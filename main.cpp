#include "smartmirror.h"


int main(void){
	int user_number;

	setServerSocket();

	getRoomLightData();
	getWeatherData();		// thread 1hour
	getOlmpicData();		// therad 1hour
	getSportsData();		// thread 1hour
	getRoomCondition();		// thread 30sec, 1min
	

	waittingSmartMirror();
	Display(display=1);
	setUser(user_number);

	while(1){
		switch (power)
		{
		case ON:
			runningSmartMirror(user_number, display=2, weather, olmpic, sports, room_condition, room_light);
			continue;
		case OFF:
			waittingSmartMirror(display=0, weather, olmpic, sports, room_condtion);
			Display(display);
			setUser(user_number);
			continue;
		}
		
	}

	return 0;
}

