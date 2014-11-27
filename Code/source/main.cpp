#include "variant.h"
#include "Arduino.h"
#include "shell.h"

void myinit();
int main() {
	myinit();

	int a = 0;

	while (1) {
		if (Serial.available() > 0)
			a = Serial.read();
		if (a != 0)
			break;
	}
	shell();

//  char* examplemainhelptext="this is mein main function";
//  addNewCommand(examplemaincommandsadd,"startmy",examplemainhelptext);

//endless loop we should never jump out
//while (1) {
//	if (Serial.available() > 0) {
//		char a = Serial.read();
//		Serial.print("I received: ");
//		Serial.println(a);
//	}
//}
	return 0;
}

void myinit() {
	Serial.begin(9600);

}

//if (Serial.available() > 0) {
//			incomingByte = Serial.read();
//			Serial.print("I received: ");
//			Serial.println(incomingByte);
//		}
//	}
