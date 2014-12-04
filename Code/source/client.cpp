#include "client.h"
#include "RFduinoGZLL.h"
#include "RFduinoBLE.h"
#include "variant.h"
#include "Arduino.h"

int interval = 675;

void clientInit(bool ble_enable){
	if(ble_enable){
	//	RFduinoBLE.advertisementData = "temp";
	  RFduinoBLE.advertisementData = "ledbtn";
	  RFduinoBLE.begin();

	}else{
		RFduinoGZLL.begin(DEVICE0);
	}
}

void RFduinoBLE_onAdvertisement(bool start)
{
  // turn the green led on if we start advertisement, and turn it
  // off if we stop advertisement

  if (start)
    Serial.println("onAdvertisement ON");
  else
  	Serial.println("onAdvertisement OFF");
}

void RFduinoBLE_onConnect()
{
	Serial.println("have connected");
}

void RFduinoBLE_onDisconnect()
{
	Serial.println("have disconnected");
}

