#include "variant.h"
#include "Arduino.h"
#include "shell.h"
#include "RFduinoBLE.h"
#include "client.h"
#include "host.h"
#include "sensor.h"
#include "ads1231.h"
#include "utils.h"


void mainInit();
void device();
void host();


void initADC(){
	ads1231_init();
	pinMode(6, OUTPUT);
	pinMode(5, OUTPUT);
	digitalWrite(5, LOW);
	digitalWrite(6, HIGH);
}

void initGSM(){

}

void setup() {
	mainInit();
	//intiADC();
}

void loop() {
	int a = 0;
	Serial.println("in main");

	while (1) {
		if (Serial.available() > 0) {
			a = Serial.read();
			if (a == 'h') {
				Serial.println("BIN host");
				host();
				break;
			}
			if (a == 'd') {
				Serial.println("BIN device");
				device();
				break;
			}
		}
	}
	shell();
}

void device() {
	clientInit(false);
}
void host() {
	hostInit(false);
	while (1)
		;
}
void someoutput() {
//	Serial.println("The device id is:");
//	uint64_t id = getDeviceId();
//	Serial.printf("%x", getDeviceIdLow());
//	Serial.printf("%x", getDeviceIdHigh());
//	Serial.println("");
//	Serial.println("high");   // turn the LED on (HIGH is the voltage level)
//	delay(1000);                     // wait for a second
//	Serial.println("low");
//	delay(1000);

	Serial.println("START normal output");
	Serial.println("");
	Serial.end();
	Serial.begin(9600,2,3);
	delay(10);
	Serial.println("ANDERER OUTPUT");
	Serial.println("");

	Serial.end();
	Serial.begin(9600);

}

void readADC() {
int weight = 0;
long int val = 0;
int ret = 0, weight_const = 0;
//ads1231_get_grams(weight);
ret = ads1231_get_value(val);
Serial.printf("return get value : %d\r\n",ret);

ret = ads1231_get_grams(weight);
Serial.printf("return get gramms : %d\r\n",ret);
//ret = ads1231_get_stable_grams(weight_const);
Serial.printf("return get const : %d\r\n",ret);
//Serial.println("LONG");
Serial.printf("get_value %x",val);
Serial.println("");
Serial.printf("get_grams %d",weight);

}

void mainInit() {
Serial.begin(9600);
addNewCommand(readTemperature, "temp",
		"read the temperaturevalue of the BLE shield");
addNewCommand(someoutput, "out", "set output low");
addNewCommand(readADC, "read", "read ADC value");

}


