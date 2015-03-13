#include "client.h"

// interval between advertisement transmissions ms (range is 20ms to 10.24s) - default 20ms
// 675 ms between advertisement transmissions

Clientclass client_;
Clientclass::Clientclass() {

}
Clientclass::Clientclass(bool ble_enable, device_t device) {
	ble_transmission = ble_enable;
	device_ = device;
	int rsi = 0;

	//int interval = 675;

	if (ble_enable) {
		// set the transmit power level in dBm (-20 dBm to +4 dBm)
		// (this will conserve battery life, and decrease the range
		// of the transmittion for near proxmity applications)
		RFduinoBLE.txPowerLevel = -20;
		RFduinoBLE.advertisementData = "data";
		RFduinoBLE.advertisementInterval = 675;
		//RFduinoBLE.advertisementInterval = 1500;
		RFduinoBLE.connectable = true;
		RFduinoBLE.deviceName = "mydeviceName";
		RFduinoBLE.begin();
	} else {
		RFduinoGZLL.txPowerLevel = -20;
		RFduinoGZLL.begin(device);

	}
}

void Clientclass::switchConnection(device_t device) {
	if (ble_transmission) {
		RFduinoBLE.end();
		RFduinoGZLL.begin(device);
		ble_transmission = false;
	} else {
		RFduinoBLE.txPowerLevel = -20;
		RFduinoBLE.advertisementData = "data";
		RFduinoBLE.advertisementInterval = 675;
		RFduinoBLE.connectable = true;
		RFduinoBLE.deviceName = "device";
		RFduinoBLE.begin();
		RFduinoGZLL.end();
		ble_transmission = true;
	}
}
device_t Clientclass::getCurrentDevice() {
	return device_;
}
int Clientclass::getRSSI() {
	return rssi;
}

void Clientclass::setRSSI(int rssi) {
	this->rssi = rssi;
}

void Clientclass::sendBLE(char* data, int len) {
	Serial.println("OS");
//	Serial.println("start to send over BLE");
	if (!RFduinoBLE.send(data, len)) {

	}
	//	Serial.println("BLE data send");
	else {

	}

	//Serial.println("could not send data");
}

void Clientclass::setError(String t) {
	error_ = String(error_) + String(t);
}

String Clientclass::getError() {
	String t = error_;
	error_ = "";
	return t;
}
String Clientclass::getTempValue(int node) {
	return temp_value_[node];
}
void Clientclass::setTempValue(String value, int node) {
	temp_value_[node] = value;
}

String Clientclass::getAdcValue(int node) {
	return adc_value_[node];
}
void Clientclass::setAdcValue(String value, int node) {
	adc_value_[node] = value;
}

void Clientclass::setTransactionActive(bool ta) {
	transaction_active_ = ta;
}
bool Clientclass::getTransactionActive() {
	return transaction_active_;
}

void Clientclass::setTransactionFinish(bool ta) {
	client_transaction_finish = ta;
}
bool Clientclass::getTransactionFinish() {
	return client_transaction_finish;
}

int Clientclass::decTransactionCount() {
	transaction_count_--;
	return transaction_count_;
}
int Clientclass::getTransactionCount() {
	return transaction_count_;
}
//***************************************************************************************
//															BLE
//***************************************************************************************
void RFduinoBLE_onAdvertisement(bool start) {

	if (start)
//		Serial.println("onAdvertisement ON");
		Serial.println("ONADV");
	else
		Serial.println("OFFADV");
		//Serial.println("onAdvertisement OFF");
}

void RFduinoBLE_onConnect() {
	Serial.println("OCON");
//	Serial.println("have connected");
	//RFduinoBLE.send("connected", 9);
}

void RFduinoBLE_onDisconnect() {
	Serial.println("ODC");
	//Serial.println("have disconnected");
}

void RFduinoBLE_onReceive(char *data, int len) {
	//Serial.printf("arecieved string with len = %d\n\r", len);
	///delay(1000);
	Serial.println("Oreceive");
	int leng = 10;
	char buf[leng];
	char ch = 'A';
	for (int i = 0; i < leng; i++) {
		buf[i] = ch;
		ch++;
	}
	client_.sendBLE(buf, leng);
//	Serial.println(client_.getRSSI());

}
// returns the dBm signal strength indicated by the receiver
// received signal strength indication (-0dBm to -127dBm)
void RFduinoBLE_onRSSI(int rssi) {
	client_.setRSSI(rssi);
	//Serial.printf("rssi %d", rssi);
	//Serial.println("");
}

//***************************************************************************************
//															GZLL
//***************************************************************************************

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
	char cp[len - 2];
	int i = 0;
	char protocol[2];

	if (len > 0) {
		for (i = 2; i < len; i++) {
			cp[i - 2] = data[i];
		}
		protocol[0] = data[0];
		protocol[1] = data[1];
	}
	if (client_.getCurrentDevice() == HOST) {
		if (len > 0) {
//			client_.setError(
//					String("recieved prot: ") + String(protocol) + String("data")
//							+ String(cp));
			if (protocol[0] == 'g') {
				Serial.println("OHS");
				client_.setError(String("client want to send data-send temp"));
				RFduinoGZLL.sendToDevice(device, "t?");
			} else if ((protocol[0] == 't') && (protocol[1] == '!')) {
				client_.setError(
						String("recieved temp -need adc value") + String("\n\r")
								+ String(cp) + String("\n\r"));
				client_.setTempValue(cp, device);
				RFduinoGZLL.sendToDevice(device, "a?");
			} else if ((protocol[0] == 'a') && (protocol[1] == '!')) {
				client_.setError(
						String("recieved adc -no more values neee") + String("\n\r")
								+ String(cp) + String("\n\r"));
				client_.setAdcValue(cp, device);
				RFduinoGZLL.sendToDevice(device, "f?");
				if (client_.decTransactionCount() == 0) {
					client_.setTransactionActive(false);
					//RFduinoGZLL.end();
				}

			}
		}
	} else {
		if (len > 0) {
			//client_.setError(
				//	String("recieved prot: ") + String(protocol) + String("data")
				//			+ String(cp));
			if ((protocol[0] == 't') && (protocol[1] == '?')) {
				client_.setError(String("host want to recieve temp\r\n"));
				int temp = 321;
				//int temp = 321;
				RFduinoGZLL.sendToHost(String("t!") + String(temp));
			} else if ((protocol[0] == 'a') && (protocol[1] == '?')) {
				client_.setError(String("host want to recieve adc\r\n"));
				int adc = 456;
				//int adc = 87654321;
				RFduinoGZLL.sendToHost(String("a!") + String(adc));
			} else if ((protocol[0] == 'f') && (protocol[1] == '?')) {
				client_.setError(String("transaction finish now\r\n"));
				client_.setTransactionActive(false);
				//Serial.println("END: send");
				client_.setTransactionFinish(true);
				RFduinoGZLL.end();
			}
		} else {
//			client_.setError(String("len=0"));
		}
	}

}
