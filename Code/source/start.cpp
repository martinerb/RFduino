#include "variant.h"
#include "Arduino.h"
#include "shell.h"
#include "RFduinoBLE.h"
#include "client.h"
#include "sms.h"
#include "SIM900.h"
#include "sensor.h"
#include "HWSerial.h"
#include "timer.h"
#include "config.h"

void mainInit();
void roleHost();
void roleClientBLE();
void initADC();
void roleClientGZLL();

#define DEVICE
//#define HOST

Clientclass client_;
void getWeigth() {
	initADC();
	delay(2000);
	client_.setTempValue(20, client_.getCurrentDevice());
	int adc = readADC();
	client_.setLoadWeight(adc, client_.getCurrentDevice());
	ADCPowerDown();
	delay(500);
}

void initGSM() {
	int i = gsm.begin(9600);
	Serial.end();
	Serial.begin(9600);
	delay(1000);
//	Serial.printf("Ende init GSM ret = %d---\r\n", i);
}

void sendSMS() {
	String sms_text;
	int i = 0;
	sms_text = "Gewichte der Waage Nummer:";
	for (i = 1; i <= NODENUMBER; i++) {
		sms_text += String(i) + String(" ist ") + String(client_.getLoadWeight((device_t) (i - 1))) + String("g");
	}
	Serial.println(sms_text);
	int ret = gsm.sendSMS(PHONE_NUMBER, sms_text.cstr());
	timerStop();
	Serial.end();
	Serial.begin(9600);
	delay(1000);
}

void startTransmission() {
	if (((client_.getCurrentDevice() == DEVICE0) || (client_.getCurrentDevice() == DEVICE1)) && (client_.getTransactionActive() == false)) {
		delay(1500);
		if (client_.getTransactionFinish() == true) {
			RFduinoGZLL_end();
			delay(1500);
			client_.setTransactionFinish(false);
//			RFduino_ULPDelay(5000);
		}
		client_.setTransactionActive(true);
		RFduinoGZLL.sendToHost("g?");
	}
	if ((client_.getCurrentDevice() == HOST) && (client_.getTransactionActive())) {
	} else if (((client_.getCurrentDevice() == DEVICE0) || (client_.getCurrentDevice() == DEVICE1)) && (client_.getTransactionActive())) {
		delay(1500);
		RFduinoGZLL.sendToHost(NULL, 0);
		delay(1500);
	} else if (client_.getCurrentDevice() == NOTDEFINED) {
	} else {
		delay(5000);
		RFduinoGZLL_end();
		delay(1500);
		initGSM();
		delay(1500);
		sendSMS();
	}
}

void setup() {
	mainInit();
	//roleClientBLE();
	//roleHost();
	//roleClientGZLL();
//	initGSM();
	shell();
}

void test() {
	Serial.println("start");
	//getWeigth();
	Serial.println(client_.getLoadWeight(client_.getCurrentDevice()));
	Serial.println("stop");
}

void loop() {
// time to next day 8AM

	RFduino_ULPDelay(SECONDS(60));
#ifndef DEVICE
	while(1) {
		roleClientGZLL();
		startTransmission();
		RFduino_ULPDelay(SECONDS(65));
		roleClientGZLL();
		startTransmission();
		RFduino_ULPDelay(SECONDS(60));
		roleClientGZLL();
		startTransmission();
		RFduino_ULPDelay(SECONDS(60));
	}
#endif

#ifndef HOST
	while (1) {
		roleHost();
		startTransmission();
		RFduino_ULPDelay(MINUTES(1));
		roleHost();
		startTransmission();
		RFduino_ULPDelay(MINUTES(1));
		roleHost();
		startTransmission();
		RFduino_ULPDelay(MINUTES(1));
	}

#endif

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
	//Serial.println("bin BLE client");
	client_ = Clientclass(true, NOTDEFINED);
}

void roleClientGZLL() {
	//client_ = Clientclass(false, DEVICE1);
	getWeigth();
	delay(1000);
	client_ = Clientclass(false, DEVICE0);
	delay(500);
}

void roleHost() {
	client_ = Clientclass(false, HOST);
	delay(1000);
	client_.setTransactionActive(true);
}

void getClientError() {
	Serial.println(client_.getError());
}

void mainInit() {
	Serial.begin(9600);

	addNewCommand(roleClientBLE, "ble", "client is ble client", true);
	addNewCommand(roleClientGZLL, "gzll", "client is gzll client", true);
	addNewCommand(roleHost, "host", "device is hostdevice for gzll", true);
	addNewCommand(send, "send", "send data", false);
	addNewCommand(getClientError, "error", "return in error stored string", false);
	addNewCommand(getWeigth, "read", "read ADC value", false);
	addNewCommand(initADC, "initadc", "read ADC value", false);
	addNewCommand(initGSM, "initgsm", "give power to gsm module", false);
	addNewCommand(test, "t", "test", false);
	delay(5000);

}

