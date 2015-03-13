#ifndef CLIENT_H
#define CLIENT_H

#include  "libRFduinoGZLL.h"
#include "RFduinoGZLL.h"
#include "RFduinoBLE.h"
#include "variant.h"
#include "Arduino.h"

#define NODENUMBER 1
class Clientclass: public virtual RFduinoGZLLClass {
private:

	bool ble_transmission;
	device_t device_;
	int rssi;
	String error_ = "";
	int weight[NODENUMBER];
	int temp_value_[NODENUMBER];
	bool transaction_active_ = false;
	bool client_transaction_finish = false;

	int transaction_count_ = NODENUMBER;

public:
	Clientclass(bool ble_enable, device_t device);
	Clientclass();
	void sendBLE(char* data, int len);
	void switchConnection(device_t device);
	device_t getCurrentDevice();
	int getRSSI();
	void setRSSI(int rssi);
	void setError(String t);
	String getError();

	int getTempValue(device_t node);
	void setTempValue(int value, device_t node);

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
