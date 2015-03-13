#include "sensor.h"
#include "shell.h"
#include "variant.h"
#include "Arduino.h"
#include "RFduinoGZLL.h"
#include "ads1231.h"
#include "config.h"

// get a cpu temperature sample
// degrees c (-198.00 to +260.00)
// degrees f (-128.00 to +127.00)

float readTemperature() {
	Serial.println("Start reading temperature");
	float temp = RFduino_temperature(CELSIUS);
	Serial.printf("%f", temp);
	return temp;
}

void initADC() {
//power supply over pin 5(GND) and 6(VDD)
	pinMode(6, OUTPUT);
	pinMode(5, OUTPUT);
	digitalWrite(5, LOW);
	digitalWrite(6, HIGH);
	delay(100);
	ads1231_init();
}

void ADCPowerDown(){
	Serial.println("start power down mode");
	digitalWrite(ADS1231_PWDN, LOW);
}


int readADC() {
	int weight = 0;
	int ret = 0, weight_const = 0;
	//Serial.println("OADCS");
	ret=ads1231_get_grams(weight);
	Serial.printf("get_value %d\n\r", weight);
	//Serial.println("OADCE");
	return weight;
}

