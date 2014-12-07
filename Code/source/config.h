/**
 * Configuration file for the evobot Arduino code
 */

#ifndef CONFIG_H
#define CONFIG_H

// emulate a scale
//#define ADS1231_EMULATION 1

#define ADS1231_DATA_PIN 4
#define ADS1231_CLK_PIN  3
#define ADS1231_PWDN 		 2
#define ADS1231_OFFSET 48748056
//#define ADS1231_OFFSET   (127.97810572652163 - 3)

#define ABORT_BTN_PIN    6
#define RESUME_BTN_PIN   5



// ADC counts per milligram
#define ADS1231_DIVISOR  1565.1671343537414
// Zero offset, grams


// How to calibrate using a weight (in grams) and the measured raw value
// as returned by ads1231_get_value():
// raw1    = -159119.02777777778
// weight1 = 0.
// raw2    = 761393.42307692312
// weight2 = 679.2
// ADS1231_DIVISOR = (raw1 - raw2) / (weight1 - weight2)
// ADS1231_OFFSET  = weight1 - (raw1 * (weight1 - weight2)) / (raw1 - raw2)

// Delay between single servo steps when turning bottle up/down, in milliseconds



// Time to weight for a stable weight on scale
// if ads1231_get_stable_grams() is called
#define ADS1231_STABLE_MILLIS 5000

// Stop pouring early in grams to account for the liquid pouring out while turning
// the bottle up
#define UPGRIGHT_OFFSET  17

// When waiting for changes of weight on scale, ignore changes less than... (in grams)
#define WEIGHT_EPSILON  2

// If ready, send READY message every x milliseconds
//#define SEND_READY_INTERVAL 2000

// Wait at least x milliseconds before calling ads1231_get_grams() again
// when turning servo, because it takes to long to call it always. Increasing
// this value makes the servo faster but the response time to the scale slower.
//#define ADS1231_INTERVAL 200

// Commands must be sent faster than SERIAL_TIMEOUT milliseconds.
#define SERIAL_TIMEOUT 50

// max length of serial commands, number of characters
//#define MAX_COMMAND_LENGTH 50

// For safety: we will never pour more than this amount at once (in grams)
// (not per bottle, but per pouring procedure)
//#define MAX_DRINK_GRAMS 250

// If the requested and the measured output for at least one ingredient is
// larger, an ERROR message will be printed instead of ENJOY (in grams).
//#define MAX_POUR_ERROR  20

#endif
