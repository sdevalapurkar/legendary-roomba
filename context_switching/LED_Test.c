#define F_CPU 16000000L		// Specify oscillator frequency
#include <avr/io.h>
#include "LED_Test.h"

/**
 * \file LED_Test.c
 * \brief Small set of test functions for controlling LEDs on a AT90USBKey
 *
 * \main page Simple set of functions to control the state of the on board
 *  LEDs on the AT90USBKey.
 *
 * \author Alexander M. Hoole
 * \date October 2006
 */

void init_LED_C6(void)
{
	DDRC |= LED_C6;		//Set LED to output (pins 4 and 5)
	PORTC = 0x00;		//Initialize port to LOW (turn off LEDs)
}

void enable_LED(unsigned int mask)
{
	PORTC = mask;		//Initialize port to high
}

void disable_LEDs(void)
{
    PORTC = 0x00;	//Initialize port to high
}
