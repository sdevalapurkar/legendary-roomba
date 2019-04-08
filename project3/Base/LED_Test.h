
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

//#define LED_D2_GREEN	0x20
//#define LED_D2_RED		0x10
#define LED_C6_GREEN	0x40
//#define LED_D5_RED		0x80
//#define LED_D2			(LED_D2_GREEN | LED_D2_RED)
#define LED_C6			(LED_C6_GREEN)

void enable_LED(unsigned int mask);
//void init_LED_D2(void);
void init_LED_C6(void);
void disable_LEDs(void);
