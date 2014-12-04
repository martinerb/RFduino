#include "host.h"
#include "RFduinoGZLL.h"

void hostInit(bool ble_enable) {
	if (ble_enable) {

	} else {
		RFduinoGZLL.begin(HOSTDEVICE);
	}

}

