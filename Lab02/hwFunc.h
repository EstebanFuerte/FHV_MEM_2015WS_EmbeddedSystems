/* hwDrv.h - Pilsan Hardware Driver Header File */

#ifndef __INChwDrv
#define __INChwDrv

#include "stdio.h"

#endif /* __INChwDrv */

int initHardware (UCHAR board);
/*	Initializes the entire hardware needed
	board: 0 = Lab exercise board in U131
	board: 1 = Focus project setup in U126
	returns file descriptor for second serial interface for the
	focus project setup in U126, or 0 if not needed
*/

char getKey (void);
/*	returns ASCII character of the key pressed
	if no key is pressed the return value is 0x0
	the function is implemented in the module kbd.c
*/

void writeToDisplay (UCHAR row, UCHAR column, char* stringBuffer);
/*	display has:
	80 columns 0 ... 79 and 
	25 rows 0 ... 24
	(position = column + 80 * row)
	only one row (or part of it) can be written at once
	in order to erase something written, overwrite it with ´ ` (blank)
*/

UCHAR readDigital (UCHAR port, UCHAR bit);
/*	port:	0x10 (PORTA)
			0x11 (PORTB)
			0x12 (PORTC)
	bit:	0 ... 7 depending on position
	return value is 0 or 1 (on the lowest bit, the other are 0)
	Note: the rotation direction can be read on PORTC, bit 1
*/

void writeDigital (UCHAR port, UCHAR bit, UCHAR value);
/*	port:	0x10 (PORTA)
			0x11 (PORTB)
			0x12 (PORTC)
	bit:	0 ... 7 depending on position
	value is 0 or 1 (on the lowest bit, the other must be 0)
*/

int readAnalog (UCHAR input, UCHAR range);
/*	input = 0 ... 7
	
	range:	0 (0 ... +5V)
			1 (-5V ... +5V)
			2 (0 ... +10V)
			3 (-10V ... +10V)
	Potentiometer = input 2, range 0
	temperature sensor = input 5, range 2; 
   	   approx. sensor sensitivity 0.4 ohm/K, between -200 and +800 °C, nominal value 100 ohm at 0 °C;
   	   sensor applied with a current of 3.12 mA; resuting sensor voltage: 0.312 V at 0 °C;
   	   input circuitry has an amplification of 15; resulting input voltage: 4.68 V at 0 °C
	motor voltage = input 1, range 3
	motor current = input 0, range 1 (or 3), sensitivity 40mA/V
	return value is 0 ... 4095 (range 0, 2), or -2048 ... 2047 (range 1, 3)
*/

void writeAnalog (UCHAR output, int value);
/*	output = 0 ... 3
	value = 0 ... 4095
	motor = ouput 0; approx. sensitivity: a change in value of  220 = a change in voltage of 1 V;
	at a value of 0 the motor will turn fastest to left, at 4095 fastest to right.
*/

int getEncoderPulses (void);
/*	returns encoder pulses since last call
	Note that the number of pulses returned differs:
	Lab exercise board in U131: 64 pulses per revolution
	Focus project setup in U126: 500 pulses per revolution
	WARNING: when it is called the first time, it will return nonsense!
	WARNING: when the motor is still the function does not return zero but the last valid value!
*/

int getEncoderPulsesZeroCorrected (void);
/*	returns encoder pulses since last call
	This function corrects the zero error of the simple function.
	Note that the number of pulses returned differs:
	Lab exercise board in U131: 64 pulses per revolution
	Focus project setup in U126: 500 pulses per revolution
	WARNING: when it is called the first time, it will return nonsense!
	WARNING: Call the function at least each 400ms on the focus project setup in U126
	         since otherwise the maximum count could be reached!  
*/

int getRotationDirection (void);
/* left = -1, right = +1 */

void motorOn (void);

void motorOff (void);

void watchdogSet (UCHAR time);
/* time given in seconds (1 ... 255) */

void watchdogTrigger (void);

void watchdogDisable (void);
