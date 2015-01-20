#include "variant.h"
#include "Arduino.h"
#include "shell.h"
#include "RFduinoBLE.h"
#include "client.h"
#include "host.h"
#include "sensor.h"
#include "ads1231.h"
#include "utils.h"
#include "sms.h"
#include "SIM900.h"

#include "HWSerial.h"

void mainInit();
void device();
void host();

Clientclass client_;

void initADC() {
	ads1231_init();
	pinMode(6, OUTPUT);
	pinMode(5, OUTPUT);
	digitalWrite(5, LOW);
	digitalWrite(6, HIGH);
}

void initGSM() {
	int i = gsm.begin(9600);
	Serial.end();
	Serial.begin(9600);
	delay(2000);
	Serial.printf("Ende init GSM ret = %d---\r\n", i);
}

void setup() {
	mainInit();
	//intiADC();
	initGSM();
}

void loop() {
	int a = 0;
	delay(1000);
	Serial.println("in main");
	shell();
}

void host() {
	hostInit(false);
	while (1)
		;
}
void someoutput() {
	Serial.println("START normal output");
	Serial.println("");
	Serial.end();

	Serial.begin(9600, 2, 3);
	delay(10);
	Serial.println("ANDERER OUTPUT");
	Serial.end();
	Serial.begin(9600);
}

void readADC() {
	int weight = 0;
	long int val = 0;
	int ret = 0, weight_const = 0;
//ads1231_get_grams(weight);
	ret = ads1231_get_value(val);
	Serial.printf("return get value : %d\r\n", ret);

	ret = ads1231_get_grams(weight);
	Serial.printf("return get gramms : %d\r\n", ret);
//ret = ads1231_get_stable_grams(weight_const);
	Serial.printf("return get const : %d\r\n", ret);
//Serial.println("LONG");
	Serial.printf("get_value %x", val);
	Serial.println("");
	Serial.printf("get_grams %d", weight);

}

void getPhonenumber(){
	char phone_num[20]; // array for the phone number string
	if (1 == gsm.GetPhoneNumber(1, phone_num)) {
		Serial.end();
		Serial.begin(9600);
		Serial.println("habe phone nummber");
		Serial.println(phone_num);

	} else {
		Serial.end();
		Serial.begin(9600);
		Serial.println("habe phone nummber");
	}

}
void sendSMS() {

	char phone_num[20]; // array for the phone number string
	char ret_val = -2;
	ret_val = gsm.sendSMS("00436603995135", "Should be last test SMS");
	Serial.end();
	Serial.begin(9600);
	delay(2000);
	Serial.printf("returnvalue = %d\n\r",ret_val);


//	char position;
//	char sms_text[100]; // array for the SMS text string

//	position = gsm.isSMSPresent(SMS_UNREAD);
//	if (position) {
//		// there is new SMS => read it
//		ret_val = gsm.getSMS(position, phone_num, sms_text, 100);
//		Serial.end();
//		Serial.begin(9600);
//		Serial.println(ret_val);
//		Serial.println(phone_num);
//		Serial.println(sms_text);
//		Serial.println(position);
//	}

}
void imei() {
	char * imei = "";
	imei = gsm.getIMEI();
	Serial.end();
	Serial.begin(9600);
	Serial.println(imei);
}
void send() {

	int len = 10;
	char buf[len];
	char ch = 'A';
	for (int i = 0; i < len; i++) {
		buf[i] = ch;
		ch++;
	}
	client_.send(buf, len);
}

void roleClientBLE() {
	Serial.println("bin BLE client");
	client_ = Clientclass(true, NOTDEFINED);

}

void roleClientGZLL() {
	Serial.println("bin GZLL client");
	client_ = Clientclass(false, DEVICE0);
}

void roleHost() {
	Serial.println("bin GZLL Host");
	//client =Client(false,HOST);
}
void powerOn(){
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);
}

void mainInit() {
	Serial.begin(9600);

	addNewCommand(roleClientBLE, "ble", "client is ble client");
	addNewCommand(roleClientGZLL, "gzll", "client is gzll client");
	addNewCommand(roleHost, "host", "device is hostdevice for gzll");
	addNewCommand(send, "send", "send data");
	addNewCommand(readTemperature, "temp",
			"read the temperaturevalue of the BLE shield");
	addNewCommand(someoutput, "out", "set output low");
	addNewCommand(readADC, "read", "read ADC value");
	addNewCommand(sendSMS, "sendsms", "sende eine sms and eingestellte nummer");
	addNewCommand(imei, "imei", "return the imei number of the modul");
	addNewCommand(powerOn, "power", "give power to gsm module");
	addNewCommand(initGSM, "initgsm", "give power to gsm module");
	addNewCommand(getPhonenumber, "number", "return first phonenumber");
	delay(5000);
	pinMode(4, OUTPUT);
	digitalWrite(4, LOW);
	Serial.println("maininit finish");

}

