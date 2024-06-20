// iot 구현(wifi 아두이노, Blu 장비 등)

#include <iostream>
#include "smartmirror.h"

int main(void){

	SmartMirror sm;
	sm.initSmartMirror(socket=True, diplay=True, analysis=True);
	
	while(power == ON){
		sm.runningSmartMirror(display(???));
	}
	
	return 0;
}
