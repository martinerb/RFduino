// TIMER core
#include "timer.h"
#include "variant.h"
#include "Arduino.h"
#include "WInterrupts.h"
#include "config.h"

boolean last_state = false;


void timer0Interrupt(void)
{
  if (NRF_TIMER0->EVENTS_COMPARE[0] != 0)
  {
  	if (last_state == false){
  		digitalWrite(CLOCK_PIN, HIGH);
  		last_state = true;
  	}else{
  		digitalWrite(CLOCK_PIN, LOW);
  		last_state = false;
  	}
    NRF_TIMER0->EVENTS_COMPARE[0] = 0;
  }
}

void timerStart(void)
{
  NRF_TIMER0->TASKS_STOP = 1;	// Stop timer
  NRF_TIMER0->MODE = TIMER_MODE_MODE_Timer;  // taken from Nordic dev zone
  NRF_TIMER0->BITMODE = (TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos);
  NRF_TIMER0->PRESCALER = 4;	// 1us resolution
  NRF_TIMER0->TASKS_CLEAR = 1; // Clear timer
  NRF_TIMER0->CC[0] = TOGGLE_INTERVAL * 1000;
  NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;  // taken from Nordic dev zone
  NRF_TIMER0->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
  attachInterrupt(TIMER0_IRQn, timer0Interrupt);    // also used in variant.cpp to configure the RTC1
  NRF_TIMER0->TASKS_START = 1;	// Start TIMER
	pinMode(CLOCK_PIN, OUTPUT);

}


void timerStop(void){
	 detachInterrupt(TIMER0_IRQn);
	 digitalWrite(CLOCK_PIN, LOW);
	 last_state = false;
}
