# The Legendary Roomba

Software for controlling a Roomba robot and some other interesting stuff! It can shoot lasers too!!!

# The Robot Itself

The legend is a semi-autonomous roomba robot for use within a game of defend your castle. The functionality built for the robot included being able to move based on joystick input over bluetooth, detect a light source, shoot a laser by adjusting a servo pan-tilt kit, detect virtual walls as well as actual barriers. The objective of the game was to either defend your castle whilst on defense or to try and shoot the opponent castles/robots down whilst on offense.

In order to build this robot, functionality from a TTA scheduler we built was used. Periodic and sporadic tasks were utilized as necessary to perform certain tasks at specific times and for specific durations. The roomba library was used to control the movement of the roomba while also allowing other functionality such as being able to play a musical tune using the robot. A uart library was used to allow for communication between our joystick remote station and the base station situated on the roomba itself. Finally, a servo library was used to help control the movement of the servo motors situated on the roomba.

## Architecture

The remote station wiring and architecture (block diagram) was as follows:

![remote](/remote_station.png)

The base station wiring and architecture (block diagram) was as follows:

![base](/base_station.png)

# Components and Programs

These are some simple programs and components used as part of the larger project in building out the functionality of the legendary roomba:

## UART Loopback

Simple program that does a "loop back" from UART0 to UART1 and back out from UART1 to UART0. Whatever you enter from your laptop will eventually echo back to the console via UART1.

## Bluetooth Loopback

Using two bluetooth modules for sending/receiving 32 characters from a base station wirelessly to another remote station and back.

<img src="bluetooth.jpg" width="500">

## LED Fade

A program which will fade an LED from minimum brightness (OFF) to maximum brightness (ON) using the PWM timer. The fading period (min to max to min) should be close to our "breathing" period, which is 20 breaths per min for an adult, or once every 3 seconds, similar to when a laptop sleeps.

## Joystick LED control

Using a PS/2 like Joystick, we calibrated the min and max reading on the X and Y axes, and identified a "dead band" (a range of values) that is very noisy (i.e., unreliable readings).

Added functionality such that at the max/min position, the LED light will "breathe" faster and at the neutral position the LED will "breathe" at a constant rate.

## Servo Motor Sweep

A simple program that sweeps the shaft of a RC servo motor back and forth across 180 degrees. Uses the Arduino servo library and its functions for this purpose.

A servo motor is controlled by a pulse varying in width from 500uSec to 2500uSec precisely and periodically. Typically, a pulse is sent once every 20 to 30 mSec. This is the base-period of a PPM control signal, which can control up to 8 servo motors in a RC-remote-controller. Each servo is controlled by a pulse ranged from 0.5mSec to 2.5mSec (500uS to 2500uS). So, for eight servo motors, we need to send 8 x 2.5mSec = 20mSec, which is the minimum based period of a PPM signal.

Center of the servo movement is at 1500 us (90 degrees) and far left and right are at 1000 us (0 degrees) and 2000 us (180 degrees) respectively.

## Timer Interrupt

A simple timer interrupt handler with a frequency of 1 KHz (once every mSec). The main function displays the elapsed time in minutes, seconds and hundredths of a second MM:SS:HH on an LCD.

## Context Switching - RTOS

A simple program that creates two tasks so that they can context-switch back and forth. This can be done voluntarily; when one yields, the other runs. Or it can be done (using our timer interrupt handler) so that the tasks switch involuntarily, once every specified time interval. This is a rudimentary round-robin scheduler.

# The Team

![team](/team.png)

- Shreyas Devalapurkar
- Rickus Senekal
