#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "scheduler.h"
#include "LED_Test.h"
#include <stdbool.h>

void pulse_pin1_task(bool is_ready_state) {
	if (is_ready_state) {
		PORTH |= 0x20;
		disable_LEDs();
		PORTH &= ~0x20;
	}
}


void pulse_pin2_task(bool is_ready_state) {
	if (is_ready_state) {
		PORTG |= 0x20;
		PORTG &= ~0x20;
		disable_LEDs();
	}
}


int main(void) {
	DDRG |= 0x20; // PWM-4
	DDRH |= 0x20; // PWM-8
	DDRB |= 0x20; // PWM-11

	init_timer();
	init_LED_C6();

	Scheduler_Init();

	Scheduler_start_periodic_task(0, 100, pulse_pin1_task, true);
	Scheduler_start_periodic_task(10, 100, pulse_pin2_task, true);
	Scheduler_Start();
}
