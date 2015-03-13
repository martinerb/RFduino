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
#include "timer.h"
#include "config.h"

void mainInit();
void roleHost();
void roleClientBLE();
void initADC();
void roleClientGZLL();

void getWeigth() {
	initADC();
	delay(1000);
	client_.setTempValue(20, client_.getCurrentDevice());
	client_.setLoadWeight(readADC(), client_.getCurrentDevice());
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
	sms_text = "Gewichte der Waage: " + String(client_.getCurrentDevice()) + "\r\n"
			+ String(client_.getLoadWeight(client_.getCurrentDevice()));
	int ret = gsm.sendSMS(PHONE_NUMBER, sms_text.cstr());
	Serial.end();
	Serial.begin(9600);
	delay(1000);
	Serial.println(ret);
}

void setup() {
	mainInit();
	//roleClientBLE();
	//roleHost();
	//roleClientGZLL();

	//initGSM();

//-------------------------
	//for SIM-module
	//timerStart();
//-------------------------
	//for ADC
	//initADC();
	shell();
}

void test() {

	getWeigth();
	Serial.println(client_.getLoadWeight(client_.getCurrentDevice()));
	//initGSM();
	//sendSMS();
//	Serial.print("print values\n\r");
//	int i = 0;
//	for (i = 0; i < NODENUMBER; i++) {
//		Serial.println(String("nodenumber: ") + String(i));
//		Serial.println(client_.getAdcValue(i));
//		Serial.println(client_.getTempValue(i));
//	}
//	int a = 0;
//
//
//	digitalWrite(4, HIGH);
//	delay(500);
//	digitalWrite(4, LOW);

//	timerStop();
//		digitalWrite(4, HIGH);
//	while (a < 10) {
//
//		Serial.println("high");
//		delay(3000);
//		digitalWrite(4, LOW);
//		Serial.println("low");
//		delay(3000);
//		digitalWrite(4, HIGH);
//		a++;
//	}
//	digitalWrite(4, LOW);

	//	ADCPowerDown();

//	long int i = 0;
//	i = i + 0xF1234FFF;
//
//	Serial.println("output comming");
//	Serial.println((uint32_t) i);
//	Serial.printf("text: %x\n\r", i);
}
void loop() {
	//shell();

	if (((client_.getCurrentDevice() == DEVICE0) || (client_.getCurrentDevice() == DEVICE1)) && (client_.getTransactionActive() == false)) {
		delay(1500);
		if (client_.getTransactionFinish() == true) {
			RFduinoGZLL_end();
			client_.setTransactionFinish(false);
			//Serial.println(client_.getError());
			delay(1500);
			//Serial.println("OCF");	//client finish
			RFduino_ULPDelay(5000);
			shell();
		}
		client_.setTransactionActive(true);
		RFduinoGZLL.sendToHost("g?");
		//Serial.println("OSH");	//send to host
	}
	if ((client_.getCurrentDevice() == HOST) && (client_.getTransactionActive())) {

	} else if (((client_.getCurrentDevice() == DEVICE0) || (client_.getCurrentDevice() == DEVICE1)) && (client_.getTransactionActive())) {
		delay(1500);
		RFduinoGZLL.sendToHost(NULL, 0);
		//Serial.println("OWD");//want data
		delay(1500);
	} else if (client_.getCurrentDevice() == NOTDEFINED) {
		//shell();
		//		Serial.println("Oon");
		//		delay(1000);
		//		Serial.println("Ooff");
		//		RFduino_ULPDelay(1000);
	} else {
		delay(3000);
		RFduinoGZLL_end();
		delay(1500);
		initGSM();
		sendSMS();
		int i = 0;
		for (i = 0; i < NODENUMBER; i++) {
			Serial.println(String("nodenumber: ") + String(i));
			Serial.println(client_.getLoadWeight((device_t) i));
			Serial.println(client_.getTempValue((device_t) i));
		}
		delay(1500);
		shell();
	}

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
	//Serial.println("GZLL client");
	//client_ = Clientclass(false, DEVICE1);
	getWeigth();
	RFduino_ULPDelay(2000);
	client_ = Clientclass(false, DEVICE0);
	delay(1000);
}

void roleHost() {
//	Serial.println("bin GZLL Host");
	client_ = Clientclass(false, HOST);
	delay(1000);
//	Serial.println("ORH");	//role host
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
	//addNewCommand(sendSMS, "sendsms", "sende eine sms and eingestellte nummer", false);
	//addNewCommand(imei, "imei", "return the imei number of the modul", false);
//	addNewCommand(powerOn, "power", "give power to gsm module", false);
	addNewCommand(initGSM, "initgsm", "give power to gsm module", false);
	//addNewCommand(getPhonenumber, "number", "return first phonenumber", false);
	addNewCommand(test, "t", "test", false);
	delay(5000);
	//Serial.println("maininit finish");
	delay(500);

}

