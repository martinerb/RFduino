#include "sensor.h"
#include "shell.h"
#include "variant.h"
#include "Arduino.h"
#include "RFduinoGZLL.h"

void readTemperature(){
	Serial.println("Start reading temperature");
	float temp = RFduino_temperature(CELSIUS);
	Serial.printf("%f",temp);

	RFduinoGZLL.sendToHost(temp);

}
