#include "variant.h"
#include "Arduino.h"
#include "shell.h"
#include "RFduinoGZLL.h"
#include "client.h"
#include "host.h"


void device();
void host();
int main() {
	Serial.begin(9600);

	int a = 0;

	while (1) {
		if (Serial.available() > 0)
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

	while (1) {
		shell();
	}
	return 0;
}


void device() {
	clientInit();
}
void host() {
	hostInit();
	while(1);
}


//if (Serial.available() > 0) {
//			incomingByte = Serial.read();
//			Serial.print("I received: ");
//			Serial.println(incomingByte);
//		}
//	}
