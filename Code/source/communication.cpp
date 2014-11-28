#include "RFduinoGZLL.h"
#include "variant.h"
#include "Arduino.h"

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
	char state = data[0];

	// this test is not needed for a single device
	Serial.println(device);
	if (device == HOSTDEVICE)
		data = 0;
	if (device == DEVICE0)
		Serial.print(data);

	// no data to piggyback on the acknowledgement sent back to the Device
	// RFduinoGZLL.sendToDevice(device, "OK");
}
