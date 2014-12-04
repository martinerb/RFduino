#include "variant.h"
#include "Arduino.h"
#include "shell.h"
#include "RFduinoBLE.h"
void mainInit();


void device();
void host();


int main() {
	mainInit();

	while (1) {
		int a = 0;

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
	return 0;
}

void device() {
	clientInit();
}
void host() {
	hostInit();
	while (1)
		;
}

void mainInit(){
	Serial.begin(9600);
	addNewCommand(readTemperature,"temp","read the temperaturevalue of the BLE shield");


}




