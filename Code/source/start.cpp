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

void mainInit();
void roleHost();
void roleClientBLE();
void initADC();
void roleClientGZLL();

void initGSM() {
	Serial.println("OGSMS");
	delay(1500);
	int i = gsm.begin(9600);
	Serial.end();
	Serial.begin(9600);
	delay(2000);
	Serial.println("OGSME");
	delay(1500);
	Serial.printf("Ende init GSM ret = %d---\r\n", i);
}

void setup() {
	mainInit();
	//Scheduler.startLoop(loop2);
	//roleClientBLE();
	//roleHost();
	//roleClientGZLL();
	//initADC();
	initGSM();
	shell();
}

void test() {
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

	timerStop();
	//	digitalWrite(4, HIGH);
//	while (a < 10) {
//
//		Serial.println("high");
//		delay(1500);
//		digitalWrite(4, LOW);
//		Serial.println("low");
//		delay(1500);
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
// Erste Messung
//	while(1){
//		RFduino_ULPDelay(500);
//		int a=0;
//		while(a < 0xFFFFF){
//			a++;
//		}
//	}

//	RFduino_ULPDelay(INFINITE);

	if (((client_.getCurrentDevice() == DEVICE0) || (client_.getCurrentDevice() == DEVICE1))
			&& (client_.getTransactionActive() == false)) {
		delay(1500);
		if (client_.getTransactionFinish() == true) {
			RFduinoGZLL_end();
			client_.setTransactionFinish(false);
			//Serial.println(client_.getError());
			delay(1500);
			Serial.println("OCF");	//client finish
			RFduino_ULPDelay(5000);
			shell();
		}
		client_.setTransactionActive(true);
		RFduinoGZLL.sendToHost("g?");
		Serial.println("OSH");	//send to host
	}
	if ((client_.getCurrentDevice() == HOST) && (client_.getTransactionActive())) {

	} else if (((client_.getCurrentDevice() == DEVICE0) || (client_.getCurrentDevice() == DEVICE1))
			&& (client_.getTransactionActive())) {
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
		Serial.println("OHF");	//Host finish
		int i = 0;
		for (i = 0; i < NODENUMBER; i++) {
			Serial.println(String("nodenumber: ") + String(i));
			Serial.println(client_.getAdcValue(i));
			Serial.println(client_.getTempValue(i));
		}
		delay(1500);
		shell();
	}

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
	Serial.println("OSMSS");
	ret_val = gsm.sendSMS("00436603995135", "Strommessung SMS senden");
	Serial.end();
	Serial.begin(9600);
	delay(2000);
	Serial.println("OSMSF");
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
	//Serial.println("bin BLE client");
	client_ = Clientclass(true, NOTDEFINED);
}

void roleClientGZLL() {
	//Serial.println("GZLL client");
	//client_ = Clientclass(false, DEVICE1);
	RFduino_ULPDelay(2000);
	client_ = Clientclass(false, DEVICE0);
	delay(1000);
}

void roleHost() {
//	Serial.println("bin GZLL Host");
	client_ = Clientclass(false, HOST);
	delay(1000);
	Serial.println("ORH");	//role host
	client_.setTransactionActive(true);
}

void powerOn() {
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);
}

void getClientError() {
	Serial.println(client_.getError());
}

void getADCValues() {
//	int a = 0;
//	while(a<5){
	int i = readADC();
	//Serial.printf("returnvalue");
	//Serial.println(i);
//	delay(3000);
//	a++;
//	}
}
void mainInit() {
	Serial.begin(9600);

	addNewCommand(roleClientBLE, "ble", "client is ble client", true);
	addNewCommand(roleClientGZLL, "gzll", "client is gzll client", true);
	addNewCommand(roleHost, "host", "device is hostdevice for gzll", true);
	addNewCommand(send, "send", "send data", false);
	addNewCommand(getClientError, "error", "return in error stored string",
	false);
	addNewCommand(getADCValues, "read", "read ADC value", false);
	addNewCommand(initADC, "initadc", "read ADC value", false);

	addNewCommand(sendSMS, "sendsms", "sende eine sms and eingestellte nummer",
	false);
	addNewCommand(imei, "imei", "return the imei number of the modul", false);
	addNewCommand(powerOn, "power", "give power to gsm module", false);
	addNewCommand(initGSM, "initgsm", "give power to gsm module", false);
	addNewCommand(getPhonenumber, "number", "return first phonenumber",
	false);
	addNewCommand(test, "t", "test", false);
	delay(5000);
	//Serial.println("maininit finish");
	delay(500);

}

