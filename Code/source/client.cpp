#include "client.h"

// interval between advertisement transmissions ms (range is 20ms to 10.24s) - default 20ms
// 675 ms between advertisement transmissions

Clientclass::Clientclass() {

}
Clientclass::Clientclass(bool ble_enable, device_t device) {
	ble_transmission = ble_enable;
	Clientclass::device = device;
	//int interval = 675;

	if (ble_enable) {
		// set the transmit power level in dBm (-20 dBm to +4 dBm)
		// (this will conserve battery life, and decrease the range
		// of the transmittion for near proxmity applications)
		RFduinoBLE.txPowerLevel = -20;
		RFduinoBLE.advertisementData = "data";
		RFduinoBLE.advertisementInterval = 675;
		RFduinoBLE.connectable = true;
		RFduinoBLE.deviceName = "mydeviceName";

		RFduinoBLE.begin();
	} else {
		RFduinoGZLL.begin(device);
	}
}

void Clientclass::send(char* data, int len) {
	Serial.println("start to send");

	while (!RFduinoBLE.send(data, len))
		;
	Serial.println("buffer send");

}

void RFduinoBLE_onAdvertisement(bool start) {

	if (start)
		Serial.println("onAdvertisement ON");
	else
		Serial.println("onAdvertisement OFF");
}

void RFduinoBLE_onConnect() {
	Serial.println("have connected");
}

void RFduinoBLE_onDisconnect() {
	Serial.println("have disconnected");
}

void RFduinoBLE_onReceive(char *data, int len) {
	int i = 0;
	Serial.printf("recieved string: %s", data);
	Serial.println("");
}

// returns the dBm signal strength indicated by the receiver
// received signal strength indication (-0dBm to -127dBm)
void RFduinoBLE_onRSSI(int rssi) {
	//Serial.printf("rssi %d", rssi);
	//Serial.println("");
}

