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

//Pins
uint8_t rightJoyYPin    = 10;  // 88 A9
uint8_t rightJoyXPin    = 9; // 87 A10
uint8_t leftJoyXPin     = 11; // 85 A12
uint8_t lightSensorPin  = 12; // 86 A11

float lightSensor_val = 0;
int speedRoomba = 0;
int radiusRoomba = 0;
int xJoy = 0;
int yJoy = 0;
bool didSporadic = 0;
bool just_shot = 0;

int speedHIGH = 0;
int speedLOW = 0;
int radiusHIGH = 0;
int radiusLOW = 0;
int xJoyHIGH = 0;
int xJoyLOW = 0;
int yJoyHIGH = 0;
int yJoyLOW = 0;
int LeftJoyButtonHIGH = 0;
int LeftJoyButtonLOW = 0;
int LeftJoyButton = 0;
int roomba_state = 0; // 0 - User Control, 1 - Stationary, 2 - Reverse, 3 - Dead
int shot_count = 0;
uint8_t bumper_hit = 0;
uint8_t river_hit = 0;
roomba_sensor_data_t sensors;


// ADC reader (analogRead)
uint16_t readADC(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0 ) | (0x07 & channel);
	ADCSRB = (ADCSRB & 0xF7) | (channel & (1 << MUX5));
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)));
	return ADCH;
}


void set_stationary(bool state){
	didSporadic = 0;
	if (state == true){
		roomba_state = 1;
	} else{
		roomba_state = 0;
	}
}


void shot_fired(bool state){
	just_shot = 0;
	PORTB &= ~0x20;
}


void pulse_pin1_task(bool is_ready_state) {
	if (is_ready_state) {
		roomba_sense();
		if (roomba_state == 0){
			light_sense();
			if (didSporadic == 0){
				Scheduler_AddSporadicTask(1500, set_stationary, true, 5);
				didSporadic = 1;
			}
			if (uart_bytes_received(CH_2) >= 10) {
				speedHIGH = uart_get_byte(0, CH_2);
				speedLOW = uart_get_byte(1, CH_2);
				speedRoomba = (speedHIGH << 8) | speedLOW;
				radiusHIGH = uart_get_byte(2, CH_2);
				radiusLOW = uart_get_byte(3, CH_2);
				radiusRoomba = (radiusHIGH << 8) | radiusLOW;
				xJoyHIGH = uart_get_byte(4, CH_2);
				xJoyLOW = uart_get_byte(5, CH_2);
				xJoy = (xJoyHIGH << 8) | xJoyLOW;
				yJoyHIGH = uart_get_byte(6, CH_2);
				yJoyLOW = uart_get_byte(7, CH_2);
				yJoy = (yJoyHIGH << 8) | yJoyLOW;
				LeftJoyButtonHIGH = uart_get_byte(8, CH_2);
				LeftJoyButtonLOW = uart_get_byte(9, CH_2);
				LeftJoyButton = (LeftJoyButtonHIGH << 8) | LeftJoyButtonLOW;
				uart_reset_receive(CH_2);
			}
			//Roomba Radius
			int radiusOffset = 20;
			
			if(radiusRoomba < radiusOffset && radiusRoomba > -radiusOffset){
				radiusRoomba = 0;
				}else{
				radiusRoomba = radiusRoomba*15.87;

				if(radiusRoomba >  2000)radiusRoomba =  1999;
				if(radiusRoomba < -2000)radiusRoomba = -1999;

				if(radiusRoomba <  2000 && radiusRoomba >=  radiusOffset)radiusRoomba = 2000-radiusRoomba+150;
				if(radiusRoomba > -2000 && radiusRoomba <= -radiusOffset)radiusRoomba = -2000-radiusRoomba-150;
			}
			
			//Roomba Speed
			int speedOffset = 20;
			float tempspeed=0.000;
			
			if(speedRoomba < speedOffset && speedRoomba > -speedOffset){
				speedRoomba = 0;
				if (radiusRoomba > radiusOffset){
					speedRoomba = 350;
					radiusRoomba = 1;
				} else if (radiusRoomba < -radiusOffset){
					speedRoomba = 350;
					radiusRoomba = -1;
				}
			}else{
				speedRoomba = speedRoomba / .25; //(normal max is 500)
				if(speedRoomba >  500)speedRoomba =  500;
				if(speedRoomba < -500)speedRoomba = -500;
			}
			Roomba_Drive(speedRoomba, radiusRoomba);
		
			if (xJoy > 150){
				adjust_pan_angle(-3);
			} else if (xJoy < 125){
				adjust_pan_angle(3);
			}
			
			if (yJoy < 125){
				adjust_tilt_angle(3);
			} else if(yJoy > 150){
				adjust_tilt_angle(-3);
			}
			
			if (LeftJoyButton == 1){
				if (just_shot == 0){
					shot_count = shot_count + 1;
					if (shot_count < 6 ){
						PORTB |= 0x20;
						Scheduler_AddSporadicTask(100, shot_fired, true, 5);
						just_shot = 1;
					} else{
						PORTB &= ~0x20;
					}
				}
				
			}
			
		} else if(roomba_state == 1){
			light_sense();
			if (didSporadic == 0){
				Scheduler_AddSporadicTask(1500, set_stationary, false, 5);
				didSporadic = 1;
			}
			if (uart_bytes_received(CH_2) >= 9) {
				radiusHIGH = uart_get_byte(2, CH_2);
				radiusLOW = uart_get_byte(3, CH_2);
				radiusRoomba = (radiusHIGH << 8) | radiusLOW;
				xJoyHIGH = uart_get_byte(4, CH_2);
				xJoyLOW = uart_get_byte(5, CH_2);
				xJoy = (xJoyHIGH << 8) | xJoyLOW;
				yJoyHIGH = uart_get_byte(6, CH_2);
				yJoyLOW = uart_get_byte(7, CH_2);
				yJoy = (yJoyHIGH << 8) | yJoyLOW;
				LeftJoyButton = uart_get_byte(8, CH_2);
				uart_reset_receive(CH_2);
			}
			
			int radiusOffset = 20;
			if(radiusRoomba < radiusOffset && radiusRoomba > -radiusOffset){
				radiusRoomba = 0;
				speedRoomba = 0;
			} else if (radiusRoomba > radiusOffset){
				speedRoomba = 350;
				radiusRoomba = 1;
			} else if (radiusRoomba < -radiusOffset){
				speedRoomba = 350;
				radiusRoomba = -1;
			}
			Roomba_Drive(speedRoomba, radiusRoomba);
			
			if (xJoy > 150){
				adjust_pan_angle(-3);
				} else if (xJoy < 125){
				adjust_pan_angle(3);
			}
			
			if (yJoy < 125){
				adjust_tilt_angle(3);
				} else if(yJoy > 150){
				adjust_tilt_angle(-3);
			}
			
			if (LeftJoyButton == 1){
				PORTB |= 0x20;
				} else{
				PORTB &= ~0x20;
			}
		} else if (roomba_state == 2){
			light_sense();
			Roomba_Drive(-400, 0);
		} else if (roomba_state == 3){
			// Play a song
			Roomba_Drive(0,0);
		}
	}
}


void change_state_to_user(bool state){
	roomba_state = 0;
}


void check_dead(bool state){
	Scheduler_DeleteSporadicTask(check_dead);
	PORTG |= 0x20;
	roomba_state = 3;
}


void light_sense() {
	lightSensor_val = readADC(lightSensorPin);
	if(lightSensor_val > 120){ // 110.83 is best without 2sec test if you've fked it up man.
		PORTH &= ~0x20;
		Scheduler_AddSporadicTask(75, check_dead, true, 5);
	} else{
		Scheduler_DeleteSporadicTask(check_dead);
		PORTH |= 0x20;
	}
}


void roomba_sense() {
	uart_putchar(149, CH_1);
	uart_putchar(2, CH_1);
	uart_putchar(7, CH_1);
	uart_putchar(13, CH_1);
	
	if (uart_bytes_received(CH_1) == 2){
		bumper_hit = uart_get_byte(0, CH_1);
		river_hit = uart_get_byte(1, CH_1);
		if (bumper_hit & 0x03 != 0 || river_hit != 0){
			roomba_state = 2;
			Scheduler_AddSporadicTask(50, change_state_to_user, true, 5);
		}
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
	DDRG |= 0x20; // PWM-4
	DDRH |= 0x20; // PWM-8
	DDRB |= 0x20; // PWM-11	

	init_timer();
	init_LED_C6();
	initADC();
	init_servos();
	sei();
	// Channel 2 - Bluetooth to Arduino Communication
	uart_init(UART_9600, CH_2);
	Scheduler_Init();
	Roomba_Init();
	Scheduler_StartPeriodicTask(0, 10, pulse_pin1_task, true);
	Scheduler_Start();
}
