#include "variant.h"
#include "Arduino.h"
#include "shell.h"
#include "RFduinoBLE.h"
#include "client.h"
#include "utils.h"
#include "sms.h"
#include "SIM900.h"
#include "sensor.h"

#include "HWSerial.h"

void mainInit();
void roleHost();
void roleClientBLE();
void initADC();
void roleClientGZLL();

void initGSM() {
	int i = gsm.begin(9600);
	Serial.end();
	Serial.begin(9600);
	delay(2000);
	Serial.printf("Ende init GSM ret = %d---\r\n", i);
}

void setup() {
	mainInit();
	//roleClientBLE();
	//roleHost();
	//roleClientGZLL();
	//initADC();
	//initGSM();

}

void test() {
	Serial.print("print values\n\r");
	int i = 0;
	for (i = 0; i < NODENUMBER; i++) {
		Serial.println(String("nodenumber: ") + String(i));
		Serial.println(client_.getAdcValue(i));
		Serial.println(client_.getTempValue(i));
	}

}
void loop() {
	//RFduino_ULPDelay(INFINITE);
	//client_ = Clientclass(false, DEVICE1);
	if ((client_.getCurrentDevice() == DEVICE1)
			&& (client_.getTransactionActive() == false)) {
		delay(15000);
		client_.setTransactionActive(true);
		RFduinoGZLL.sendToHost("g?");
	}
	if ((client_.getCurrentDevice() == HOST)
			&& (client_.getTransactionActive())) {

	} else if (((client_.getCurrentDevice() == DEVICE0)
			|| (client_.getCurrentDevice() == DEVICE1))
			&& (client_.getTransactionActive())) {
		delay(1000);
		RFduinoGZLL.sendToHost(NULL, 0);
		delay(1000);
	} else if (client_.getCurrentDevice() == NOTDEFINED) {
		shell();
	} else {
		shell();
	}
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

void getPhonenumber() {
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
	Serial.printf("returnvalue = %d\n\r", ret_val);

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
	client_.sendBLE(buf, len);
}

void roleClientBLE() {
	Serial.println("bin BLE client");
	client_ = Clientclass(true, NOTDEFINED);
}

void roleClientGZLL() {
	Serial.println("GZLL client");
	client_ = Clientclass(false, DEVICE0);
	delay(1000);
	client_.setTransactionActive(true);
	RFduinoGZLL.sendToHost("g?");
}

void roleHost() {
	Serial.println("bin GZLL Host");
	client_ = Clientclass(false, HOST);
	delay(1000);
	client_.setTransactionActive(true);
}

void powerOn() {
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);
}

void getClientError() {
	Serial.println(client_.getError());
}
void mainInit() {
	Serial.begin(9600);

	addNewCommand(roleClientBLE, "ble", "client is ble client", false);
	addNewCommand(roleClientGZLL, "gzll", "client is gzll client", true);
	addNewCommand(roleHost, "host", "device is hostdevice for gzll", true);
	addNewCommand(send, "send", "send data", false);
	addNewCommand(getClientError, "error", "return in error stored string",
	false);
	addNewCommand(someoutput, "out", "set output low", false);
//addNewCommand(readADC, "read", "read ADC value");
	addNewCommand(sendSMS, "sendsms", "sende eine sms and eingestellte nummer",
	false);
	addNewCommand(imei, "imei", "return the imei number of the modul", false);
	addNewCommand(powerOn, "power", "give power to gsm module", false);
	addNewCommand(initGSM, "initgsm", "give power to gsm module", false);
	addNewCommand(getPhonenumber, "number", "return first phonenumber",
	false);
	addNewCommand(test, "test", "test", false);
	delay(5000);
	Serial.println("maininit finish");
	delay(500);

}

