#ifndef CLIENT_H
#define CLIENT_H

#include "libRFduinoGZLL.h"
#include "RFduinoGZLL.h"
#include "RFduinoBLE.h"
#include "variant.h"
#include "Arduino.h"
#include "../source/config.h"

class Clientclass: public virtual RFduinoGZLLClass {
private:

	bool ble_transmission;
	device_t device_ = DEVICE0;
	int rssi;
	String error_ = "";
	bool transaction_active_ = false;
	bool client_transaction_finish = false;

	int transaction_count_;
public:
	Clientclass(bool ble_enable, device_t device);
	Clientclass();
	void sendBLE(char* data, int len);
	void switchConnection(device_t device);
	device_t getCurrentDevice();

	void setError(String t);
	String getError();
	int getTempValue(device_t node);
	void setTempValue(int value, device_t node);
	int getRSSI();
	void setRSSI(int rssi);
	int getLoadWeight(device_t node);
	void setLoadWeight(int value, device_t node);
	bool getTransactionActive();
	void setTransactionActive(bool ta);
	bool getTransactionFinish();
	void setTransactionFinish(bool ta);
	int getTransactionCount();
	int decTransactionCount();

};
extern Clientclass client_;

#endif
