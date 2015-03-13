/**
 * Configuration file for the evobot Arduino code
 */

#ifndef CONFIG_H
#define CONFIG_H

//ADC
#define ADS1231_DATA_PIN 2
#define ADS1231_CLK_PIN  3
#define ADS1231_PWDN 		 4


//GSM
#define CLOCK_PIN				 4
#define TOGGLE_INTERVAL 100

//#define ADS1231_DIVISOR  1565.1671343537414
// Zero offset, grams
//#define ADS1231_OFFSET   (127.97810572652163 - 3)
#define ADS1231_OFFSET (127.97810572652163 - 85)
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




// Time to weight for a stable weight on scale
// if ads1231_get_stable_grams() is called
#define ADS1231_STABLE_MILLIS 5000


// Commands must be sent faster than SERIAL_TIMEOUT milliseconds.
#define SERIAL_TIMEOUT 50


#endif
