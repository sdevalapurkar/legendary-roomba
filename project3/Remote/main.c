#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "scheduler.h"
#include "LED_Test.h"
#include "roomba.h"
#include "uart.h"
#include <stdbool.h>

//PRESETS
uint16_t joyMax = 1023;
uint16_t joyMin = 0;
#define LOW_BYTE(v)   ((unsigned char) (v))
#define HIGH_BYTE(v)  ((unsigned char) (((unsigned int) (v)) >> 8))

//Pins
uint8_t rightJoyYPin    = 10;  // 88 A9
uint8_t rightJoyXPin    = 9; // 87 A10
uint8_t leftJoyYPin     = 12; // 86 A11
uint8_t leftJoyXPin     = 11; // 85 A12

//Initialization
uint32_t rightXAxisCenter = 0;
uint32_t rightYAxisCenter = 0;
uint32_t leftXAxisCenter  = 0;
uint32_t leftYAxisCenter  = 0;

// Joystick Axis
int rightXAxis      = 0;
int rightYAxis      = 0;
int leftXAxis       = 0;
int leftYAxis		= 0;
int speedRoomba		= 0;
int radiusRoomba	= 0;
int servoPan		= 0;
int servoTilt		= 0;
int leftJoyButton	= 0;


// ADC reader (analogRead)
uint16_t readADC(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0 ) | (0x07 & channel);
	ADCSRB = (ADCSRB & 0xF7) | (channel & (1 << MUX5));
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)));
	return ADCH;
}

void getJoyCenter() {
	int repeat = 30; //repeat number of times at initialization
	
	for(int i=0; i < repeat; i++){
		leftXAxisCenter  += readADC(leftJoyXPin);
		leftYAxisCenter  += readADC(leftJoyYPin);
		rightXAxisCenter += readADC(rightJoyXPin);
		rightYAxisCenter += readADC(rightJoyYPin);
	}

	leftXAxisCenter  /= repeat;
	leftYAxisCenter  /= repeat;
	rightXAxisCenter /= repeat;
	rightYAxisCenter /= repeat;
}

void pulse_pin1_task(bool is_ready_state) {
	if (is_ready_state) {
		leftXAxis = readADC(leftJoyXPin); // Actually the right one
		leftYAxis = readADC(leftJoyYPin);
		rightXAxis = readADC(rightJoyXPin); // Actually the left one
		rightYAxis = readADC(rightJoyYPin);
		
		if (PING & (1 << PG5)){
			PORTH &= ~0x20;
			leftJoyButton = 0;
			} else{
			PORTH |= 0x20;
			leftJoyButton = 1;
		}
		
		//Roomba Radius
		int radiusOffset = 20;
		//0 is straight
		//2000 (+and-) are next increment, (+/-)1 is fastest spin

		servoPan = rightXAxis;
		servoTilt = rightYAxis;
		
		radiusRoomba = (leftXAxis - leftXAxisCenter);
		if(radiusRoomba < radiusOffset && radiusRoomba > -radiusOffset){
			radiusRoomba = 0;
		}else{
			radiusRoomba = radiusRoomba*15.87;

			if(radiusRoomba >  2000)radiusRoomba =  1999;
			if(radiusRoomba < -2000)radiusRoomba = -1999;

			if(radiusRoomba <  2000 && radiusRoomba >=  radiusOffset)radiusRoomba = 2000-radiusRoomba+150;
			if(radiusRoomba > -2000 && radiusRoomba <= -radiusOffset)radiusRoomba = -2000-radiusRoomba-150;
		}
		
		int speedOffset = 20;
		float tempspeed=0.000;
		speedRoomba = -(leftYAxis - leftYAxisCenter);
		if(speedRoomba < speedOffset && speedRoomba > -speedOffset){
			speedRoomba = 0;
			}else{
			speedRoomba = speedRoomba / .25; //(normal max is 500)
			if(speedRoomba >  500)speedRoomba =  500;
			if(speedRoomba < -500)speedRoomba = -500;
		}
		
		uart_putchar(HIGH_BYTE(speedRoomba), CH_2);
		uart_putchar(LOW_BYTE(speedRoomba), CH_2);
		
		uart_putchar(HIGH_BYTE(radiusRoomba), CH_2);
		uart_putchar(LOW_BYTE(radiusRoomba), CH_2);
		
		uart_putchar(HIGH_BYTE(servoPan), CH_2);
		uart_putchar(LOW_BYTE(servoPan), CH_2);
		
		uart_putchar(HIGH_BYTE(servoTilt), CH_2);
		uart_putchar(LOW_BYTE(servoTilt), CH_2);
		
		uart_putchar(HIGH_BYTE(leftJoyButton), CH_2);
		uart_putchar(LOW_BYTE(leftJoyButton), CH_2);
	}
}

void initADC() {
	DDRC = (DDRC & 0xF0);
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz
	ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
	ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading
	ADCSRA |= (1 << ADEN);  // Enable ADC
	ADCSRA |= (1 << ADSC); //Start a conversion to warmup the ADC.
}


int main(void) {
	
	DDRG |= (1 << PG5); // PWM-4
	PORTG = 0xFF;
	DDRH |= 0x20; // PWM-8
	DDRB |= 0x20; // PWM-11
	DDRL |= 0x00; // Dig-43
	
	init_timer();
	init_LED_C6();
	initADC();
	getJoyCenter(); // init Joystick Center values
	sei();
	uart_init(UART_9600, CH_2);
	Scheduler_Init();
	Roomba_Init();
	Scheduler_StartPeriodicTask(0, 10, pulse_pin1_task, true);
	Scheduler_Start();
}
