#include "host.h"
#include "RFduinoGZLL.h"

void hostInit(){
	RFduinoGZLL.begin(HOSTDEVICE);
}



