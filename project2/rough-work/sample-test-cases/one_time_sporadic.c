#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "scheduler.h"
#include "LED_Test.h"
#include <stdbool.h>

// this should never run (AKA: LED never on)
void pulse_pin3_task(bool is_ready_state) {
	if (is_ready_state) {
		enable_LED(LED_C6_GREEN);
		PORTH |= 0x20;
		PORTH &= ~0x20;
	}
}


void pulse_pin1_task(bool is_ready_state) {
	if (is_ready_state) {
		PORTG |= 0x20;
		PORTG &= ~0x20;
	}
}


void pulse_pin2_task(bool is_ready_state) {
	if (is_ready_state) {
		PORTB |= 0x20;
		PORTB &= ~0x20;
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

	Scheduler_StartPeriodicTask(0, 100, pulse_pin1_task, true);
	Scheduler_StartPeriodicTask(150, 100, pulse_pin2_task, true);

	Scheduler_add_time_critical_sporadic_task(150, pulse_pin3_task, true, 5);
    Scheduler_add_time_critical_sporadic_task(450, pulse_pin3_task, true, 5);

	Scheduler_Start();
}
