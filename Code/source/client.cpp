#include "client.h"
#include "config.h"

// interval between advertisement transmissions ms (range is 20ms to 10.24s) - default 20ms
// 675 ms between advertisement transmissions

int weight[NODENUMBER];
int temp_value_[NODENUMBER];

Clientclass::Clientclass() {
	//device_ = DEVICE;
}
Clientclass::Clientclass(bool ble_enable, device_t device) {
	ble_transmission = ble_enable;
	device_ = device;
	int rsi = 0;

	if (ble_enable) {
		// set the transmit power level in dBm (-20 dBm to +4 dBm)
		// (this will conserve battery life, and decrease the range
		// of the transmittion for near proxmity applications)
		RFduinoBLE.txPowerLevel = -20;
		RFduinoBLE.advertisementData = "data";
		RFduinoBLE.advertisementInterval = 10000;
		RFduinoBLE.connectable = true;
		RFduinoBLE.deviceName = "mydeviceName";
		RFduinoBLE.begin();
	} else {
		RFduinoGZLL.txPowerLevel = -20;
		RFduinoGZLL.begin(device);
	}
}

//only for testing
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
//	Serial.println("OS");
	if (!RFduinoBLE.send(data, len)) {
	} else {
	}
}

void Clientclass::setError(String t) {
	error_ = String(error_) + String(t);
}

String Clientclass::getError() {
	String t = error_;
	error_ = "";
	return t;
}
int Clientclass::getTempValue(device_t node) {
	return temp_value_[node];
}
void Clientclass::setTempValue(int value, device_t node) {
	temp_value_[node] = value;
}
int Clientclass::getLoadWeight(device_t node) {
	return weight[node];
}
void Clientclass::setLoadWeight(int value, device_t node) {
	weight[node] = value;
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
	if (start) {
	} else {
	}
}

void RFduinoBLE_onConnect() {
	int leng = 10;
	char buf[leng];
	char ch = 'A';
	for (int i = 0; i < leng; i++) {
		buf[i] = ch;
		ch++;
	}
	client_.sendBLE(buf, leng);

}

void RFduinoBLE_onDisconnect() {
//	Serial.println("ODC");
}

void RFduinoBLE_onReceive(char *data, int len) {

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
	int val;
	char protocol[2];
	String res = "";

	if (len > 0) {
		for (i = 2; i < len; i++) {
			cp[i - 2] = data[i];
		}
		protocol[0] = data[0];
		protocol[1] = data[1];
	}
	if (client_.getCurrentDevice() == HOST) {
		if (len > 0) {
			if (protocol[0] == 'g') {
				Serial.println("OHS");
				client_.setError(String("client want to send data-send temp"));
				RFduinoGZLL.sendToDevice(device, "t?");
			} else if ((protocol[0] == 't') && (protocol[1] == '!')) {
				client_.setError(String("recieved temp -need adc value") + String("\n\r") + String(cp) + String("\n\r"));
				int index = String(cp).indexOf(" ");
				client_.setTempValue((int) cp, device);
				RFduinoGZLL.sendToDevice(device, "a?");
			} else if ((protocol[0] == 'a') && (protocol[1] == '!')) {
				client_.setError(String("recieved adc -no more values neee") + String("\n\r") + String(cp) + String("\n\r"));
				int index = String(cp).indexOf(" ");
				val = (int) cp;
				if (index != -1) {
					val = (String(cp).substring(0, index)).toInt();
				}
				client_.setLoadWeight(val,device);
				RFduinoGZLL.sendToDevice(device, "f?");
				if (client_.decTransactionCount() == 0) {
					client_.setTransactionActive(false);
				}
			}
		}
	} else {
		if (len > 0) {
			if ((protocol[0] == 't') && (protocol[1] == '?')) {
				client_.setError(String("host want to recieve temp\r\n"));
				client_.setError(String("t!") + String(client_.getTempValue(client_.getCurrentDevice())));
				RFduinoGZLL.sendToHost((String("t!") + String(client_.getTempValue(client_.getCurrentDevice()))).cstr());
			} else if ((protocol[0] == 'a') && (protocol[1] == '?')) {
				client_.setError(String("host want to recieve adc\r\n"));
				RFduinoGZLL.sendToHost((String("a!") + String(client_.getLoadWeight(client_.getCurrentDevice()))).cstr());
			} else if ((protocol[0] == 'f') && (protocol[1] == '?')) {
				client_.setError(String("transaction finish now\r\n"));
				client_.setTransactionActive(false);
				client_.setTransactionFinish(true);
				RFduinoGZLL.end();
			}
		} else {
		}
	}
}
