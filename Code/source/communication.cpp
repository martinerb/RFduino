#include "RFduinoGZLL.h"
#include "variant.h"
#include "Arduino.h"

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {

	// this test is not needed for a single device
	Serial.println(device);
	if (device == HOSTDEVICE)
		for (int i = 0; i < len; i++)
			Serial.print(data[i]);
	if (device == DEVICE0) {
		Serial.println("DATA RECIEVED");
		Serial.print(data);
		for (int i = 0; i < len; i++)
			Serial.printf("%d", (int)data[i]);
	}
	RFduinoGZLL.sendToDevice(device, "OK\r\n");
}
