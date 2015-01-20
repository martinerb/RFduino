#include  "libRFduinoGZLL.h"
#include "RFduinoGZLL.h"
#include "RFduinoBLE.h"
#include "variant.h"
#include "Arduino.h"

class Clientclass {

private:
	int interval;
	bool ble_transmission;
	device_t device;

	//~Clientclass();


public:
	Clientclass(bool ble_enable, device_t device);
	Clientclass();
	Clientclass getBLEInstance();
	void send(char* data, int len);

};

