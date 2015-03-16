// TIMER core
#include "timer.h"
#include "variant.h"
#include "Arduino.h"
#include "WInterrupts.h"
#include "config.h"

int timer_counter = 0;

void timer0Interrupt(void)
{

	if (NRF_TIMER0->EVENTS_COMPARE[0] != 0)
	{
		timer_counter++;
		if (timer_counter == 1)
		{
			digitalWrite(CLOCK_PIN, LOW);
		}
		else if (timer_counter > 16 )
		{
			digitalWrite(CLOCK_PIN, HIGH);
			if (timer_counter > 20)
			{
				timer_counter = 0;
			}
		}
		NRF_TIMER0->EVENTS_COMPARE[0] = 0;
	}

}

void timerStart(void)
{
	NRF_TIMER0->TASKS_STOP = 1;	// Stop timer
	NRF_TIMER0->MODE = TIMER_MODE_MODE_Timer;  // taken from Nordic dev zone
	NRF_TIMER0->BITMODE = (TIMER_BITMODE_BITMODE_32Bit
			<< TIMER_BITMODE_BITMODE_Pos);
	NRF_TIMER0->PRESCALER = 4;	// 1us resolution
	NRF_TIMER0->TASKS_CLEAR = 1; // Clear timer
	NRF_TIMER0->CC[0] = TOGGLE_INTERVAL * 1000;
	NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled
			<< TIMER_INTENSET_COMPARE0_Pos;  // taken from Nordic dev zone
	NRF_TIMER0->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled
			<< TIMER_SHORTS_COMPARE0_CLEAR_Pos);
	attachInterrupt(TIMER0_IRQn, timer0Interrupt); // also used in variant.cpp to configure the RTC1
	NRF_TIMER0->TASKS_START = 1;	// Start TIMER
	pinMode(CLOCK_PIN, OUTPUT);
	timer_counter = 0;
}

void timerStop(void)
{
	detachInterrupt(TIMER0_IRQn);
	digitalWrite(CLOCK_PIN, LOW);
	pinMode(CLOCK_PIN,INPUT);
	timer_counter = 0;
}
