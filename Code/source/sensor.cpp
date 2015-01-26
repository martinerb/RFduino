#include "sensor.h"
#include "shell.h"
#include "variant.h"
#include "Arduino.h"
#include "RFduinoGZLL.h"
#include "ads1231.h"

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
	ads1231_init();
	pinMode(6, OUTPUT);
	pinMode(5, OUTPUT);
	digitalWrite(5, LOW);
	digitalWrite(6, HIGH);
}

int readADC() {
	int weight = 0;
	long int val = 0;
	int ret = 0, weight_const = 0;
//ads1231_get_grams(weight);
	ret = ads1231_get_value(val);
	Serial.printf("return get value : %d\r\n", ret);

	ret = ads1231_get_grams(weight);
	Serial.printf("return get gramms : %d\r\n", ret);
	Serial.printf("return get const : %d\r\n", ret);
	Serial.printf("get_value %x", val);
	Serial.println("");
	Serial.printf("get_grams %d", weight);

	return weight;
}

