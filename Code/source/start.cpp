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

//#define DEVICE
#define HOSTDEVICE

int counter;
int day;

typedef struct {
	int time; //1 morning 2 noon 3 evening
	int day;
	int load_cell; //which load cell
	int temp;
	int weight;
} saved_data;

saved_data data[DATANUMBER];

Clientclass client_;
void getWeigth() {
	initADC();
	delay(2000);
	client_.setTempValue(20, client_.getCurrentDevice());
	int adc = readADC();
	Serial.println(adc);
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
	sms_text = "Gewichte der Waagen\r\n ";
//	for (i = 1; i <= NODENUMBER; i++) {
//		sms_text += String(i) + String(" ist ") + String(client_.getLoadWeight((device_t) (i - 1))) + String("g");
//	}
	int tmp = 0;
	if ((counter - 1) >= 3) {
		tmp = (counter - 1) - 3;
	} else {
		tmp = 0;
	}

	//for (i = 1; i <= NODENUMBER; i++) {
		for (i = (counter - 1); i >= tmp; i--) {
			sms_text += String("day: ") + String(data[i].day) + String(" Load Cell: ") + String(data[i].load_cell) + String(" time: ")
					+ String(data[i].time) //+ String("temp: ") + String(data[i].temp)
					+ String(" weight: ") + String(data[i].weight);
		}
	//}

	Serial.println(sms_text);
	//initGSM();
	delay(2000);
	//int ret = gsm.sendSMS(PHONE_NUMBER, sms_text.cstr());
	timerStop();
	Serial.end();
	Serial.begin(9600);
	delay(1000);
}

void startTransmission() {
//	RFduinoGZLL.begin(client_.getCurrentDevice());
//	Serial.println(client_.getCurrentDevice());
//	if (client_.getCurrentDevice() == HOST) {
//		client_.setTransactionActive(true);
//	}
	while (1) {
		if (((client_.getCurrentDevice() == DEVICE0) || (client_.getCurrentDevice() == DEVICE1)) && (client_.getTransactionActive() == false)) {
			delay(1500);
			if (client_.getTransactionFinish() == true) {
				RFduinoGZLL_end();
				delay(1500);
				client_.setTransactionFinish(false);
//			RFduino_ULPDelay(5000);
				return;
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
			//sendSMS();
			return;
		}
	}
}

void setup() {
	mainInit();
	counter = 0;
	day = 1;
	//roleClientBLE();
	//roleHost();
//	roleClientGZLL();
	//initGSM();

	shell();
}

void test() {
//	Serial.println("start");
//	//getWeigth();
//	Serial.println(client_.getLoadWeight(client_.getCurrentDevice()));
//	Serial.println("stop");
	int i = 0;
	for (i = 0; i < counter; i++) {
		Serial.println(String("day: ") + String(data[i].day) + String(" time: ") + String(data[i].time) //+ String("temp: ") + String(data[i].temp)
				+ String(" weight: ") + String(data[i].weight) + String("\r\n"));
	}

}

void saveData(int time) {
	int i = 0;
	for (i = 0; i < NODENUMBER; i++) {
		if (counter > DATANUMBER) {
			counter = 0;
		}

		data[counter].load_cell = i+1;
		data[counter].temp = client_.getTempValue((device_t) (i));
		data[counter].weight = client_.getLoadWeight((device_t) (i));
		data[counter].time = time;
		data[counter].day = day;
		counter++;
	}
}

void loop() {
// time to next day 8AM

	//RFduino_ULPDelay(SECONDS(60));
#ifdef DEVICE
//	client_.setLoadWeight(222, DEVICE1);
//	client_.setTempValue(333, DEVICE1);
	startTransmission();
//	shell();
//	Serial.println("ende");
	RFduino_ULPDelay(SECONDS(20));

//	client_.setLoadWeight(444, DEVICE1);
	roleClientGZLL();
//		Serial.println("2::");
	startTransmission();
	RFduino_ULPDelay(SECONDS(20));

//	client_.setLoadWeight(666, DEVICE1);
	roleClientGZLL();
//		Serial.println("3::");
	startTransmission();
	RFduino_ULPDelay(SECONDS(200));

	Serial.println("day over");
	roleClientGZLL();
#endif

#ifdef HOSTDEVICE
	//roleHost();
	//	Serial.println("1");
	startTransmission();
	saveData(1);
	RFduino_ULPDelay(SECONDS(10));
	roleHost();
	startTransmission();
	saveData(2);
	RFduino_ULPDelay(SECONDS(10));
	roleHost();
	startTransmission();
	saveData(3);
//	test();
	Serial.println("start send sms");
	sendSMS();
	day++;
	//RFduino_ULPDelay(SECONDS(10));
	roleHost();
	Serial.println("day off");
	//shell();
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
	getWeigth();
	delay(1000);
	client_ = Clientclass(false, DEVICE0);
//	client_ = Clientclass(false, DEVICE1);
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

