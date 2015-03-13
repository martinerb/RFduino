/**
 * Library for the TI ADS1231 24-Bit Analog-to-Digital Converter
 *
 * Written in plain C, but for some reason ino expects it to have .cpp
 * file extension, otherwise linking fails.
 *
 * File is written in plain C, because rfjakob insists on it. He thinks the
 * following comment suffices to end the discussion and to have it in plain C:
 *
 * Originally written by lumbric, commmented and slightly modified by
 * rfjakob
 *
 * But the discussion is not over yet. We are going to continue it in
 * ../bottle/bottle.h! :)
 */

#include "Arduino.h"
#include "ads1231.h"
#include "config.h"

unsigned long ads1231_last_millis = 0;

/*
 * Initialize the interface pins
 */
void ads1231_init(void) {
	Serial.println("start ADC init");
	pinMode(ADS1231_PWDN, OUTPUT);
	digitalWrite(ADS1231_PWDN, LOW);
	delay(1);
	digitalWrite(ADS1231_PWDN, HIGH);
	// We send the clock
	pinMode(ADS1231_CLK_PIN, OUTPUT);
	// ADS1231 sends the data
	pinMode(ADS1231_DATA_PIN, INPUT);
	// Enable pullup to get a consistent state in case of disconnect
	digitalWrite(ADS1231_DATA_PIN, HIGH);
	// Set CLK low to get the ADS1231 out of suspend
	digitalWrite(ADS1231_CLK_PIN, LOW);
	delay(500);
}

/*
 * Get the raw ADC value. Can block up to 100ms in normal operation.
 * Returns 0 on success, an error code otherwise (see ads1231.h)
 */
errv_t ads1231_get_value(long& val) {
	int i = 0;
	unsigned long start;

	/* A high to low transition on the data pin means that the ADS1231
	 * has finished a measurement (see datasheet page 13).
	 * This can take up to 100ms (the ADS1231 runs at 10 samples per
	 * second!).
	 * Note that just testing for the state of the pin is unsafe.
	 */
	start = millis();
	while (digitalRead(ADS1231_DATA_PIN) != HIGH) {
		if (millis() > start + 150)
			return ADS1231_TIMEOUT_HIGH; // Timeout waiting for HIGH
	}
	start = millis();
	while (digitalRead(ADS1231_DATA_PIN) != LOW) {
		if (millis() > start + 150)
			return ADS1231_TIMEOUT_LOW; // Timeout waiting for LOW
	}
	ads1231_last_millis = millis();

	// Read 24 bits
	for (i = 23; i >= 0; i--) {
		digitalWrite(ADS1231_CLK_PIN, HIGH);
		delayMicroseconds(1000);
		val = (val << 1) + digitalRead(ADS1231_DATA_PIN);
		digitalWrite(ADS1231_CLK_PIN, LOW);
		delayMicroseconds(1000);
	}

	/* Bit 23 is acutally the sign bit. Shift by 8 to get it to the
	 * right position (31), divide by 256 to restore the correct value.
	 */
	val = (val << 8) / 256;
	/* The data pin now is high or low depending on the last bit that
	 * was read.
	 * To get it to the default state (high) we toggle the clock one
	 * more time (see datasheet page 14 figure 19).
	 */
	digitalWrite(ADS1231_CLK_PIN, HIGH);
	delayMicroseconds(100);
	digitalWrite(ADS1231_CLK_PIN, LOW);
	return 0; // Success
}

/*
 * Get the weight in grams. Can block up to 100ms in normal
 * operation because the ADS1231 makes only 10 measurements per second.
 * Returns 0 on sucess, an error code otherwise (see errors.h)
 */
errv_t ads1231_get_grams(int& grams) {
	// a primitive emulation using a potentiometer attached to pin A0
	// returns a value between 0 and 150 grams
	int ret;
	long raw;
	grams = 0; // On error, grams should always be zero

	ret = ads1231_get_value(raw);
	if (ret != 0)
		return ret; // Scale error

	grams = raw / ADS1231_DIVISOR + ADS1231_OFFSET;
	return 0; // Success
}

/*
 * Get the weight in grams but measure often until the same weight is measured
 * for 3 three times.
 * Can block for longer if the weight on scale is not stable. Something similar
 * is implemented in Bottle::turn_to() (see parameter 'stable_weight') in order
 * to get a stable weight without blocking the movement of the bottle.
 *
 * Returns 0 on sucess, an error code otherwise (see errors.h)
 */
//errv_t ads1231_get_stable_grams(int& grams) {
//	grams = 0; // needs to be 0 on error
//	int i = 0;
//	unsigned long start = millis();
//	int weight_last, weight;
//	//RETURN_IFN_0(ads1231_get_grams(weight));
//	errv_t t = ads1231_get_grams(weight);
//	while (i < 2) {
//		delay(100); // TODO make this a constant in config.h
//		weight_last = weight;
//
//		//RETURN_IFN_0(ads1231_get_grams(weight));
//		t = ads1231_get_grams(weight);
//		// TODO maybe this would be more correct...? do we have abs?
//		//if (abs((weight_last - weight) < WEIGHT_EPSILON)
//		if (weight_last == weight) {
//			// weight stable
//			i++;
//		} else {
//			// weight not stable
//			i = 0;
//		}
//		DEBUG_START();
//		DEBUG_MSG("Not stable: ");
//		DEBUG_VAL(weight);
//		DEBUG_VAL(weight_last);
//		DEBUG_END();
//
//		if (millis() - start > ADS1231_STABLE_MILLIS) {
//			return ADS1231_STABLE_TIMEOUT;
//		}
//	}
//	grams = weight;
//	return 0;
//}
