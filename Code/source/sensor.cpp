#include "sensor.h"
#include "shell.h"
#include "variant.h"
#include "Arduino.h"
#include "RFduinoGZLL.h"

// get a cpu temperature sample
// degrees c (-198.00 to +260.00)
// degrees f (-128.00 to +127.00)
void readTemperature(){
	Serial.println("Start reading temperature");
	float temp = RFduino_temperature(CELSIUS);
	Serial.printf("%f",temp);

	RFduinoGZLL.sendToHost(temp);

}
