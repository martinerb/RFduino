/*
 * Library for the TI ADS1231 24-Bit Analog-to-Digital Converter
 */

#ifndef ADS1231_H
#define ADS1231_H

typedef unsigned char errv_t;
extern unsigned long ads1231_last_millis;

void ads1231_init(void);
errv_t ads1231_get_value(long& val);
errv_t ads1231_get_grams(int& grams);
//errv_t ads1231_get_stable_grams(int& grams);


#define ADS1231_TIMEOUT_HIGH         101   // Timeout waiting for HIGH
#define ADS1231_TIMEOUT_LOW          100   // Timeout waiting for LOW
//#define ADS1231_STABLE_TIMEOUT       103   // weight not stable within timeout

#endif
