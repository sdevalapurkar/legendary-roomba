/**
 * \file LED_Test.h
 * \brief Constants and functions for controlling LEDs on a AT90USBKey
 *
 * \main page Constants and functions for controlling the state of the on board
 *  LEDs on the AT90USBKey.
 *
 * \author Alexander M. Hoole
 * \date October 2006
 */

#define LED_C6_GREEN	0x40
#define LED_C6			(LED_C6_GREEN)

void enable_LED(unsigned int mask);
void init_LED_C6(void);
void disable_LEDs(void);
