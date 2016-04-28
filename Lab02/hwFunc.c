/* hwFunc.c - Pilsan Hardware Access Functions Source File */

/*
DESCRIPTION
This is a file that implements basic functions needed to acees the 
board's hardware.
Functions are not reentrant!

Include files needed in the same directory:
  - hw.h: general hardware defines
  - disp.h: display header file
  
Source files needed in the same directory:
  - kbd.c: keyboard source file
  - disp.c: display source file
*/

/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <sysLib.h>
#include <ioLib.h>

#include "hw.h"
#include "disp.h"

extern DISP_CON_DEV dispConDv;

unsigned char pulsCnt = 0;

int initHardware (UCHAR board)
/* Initializes the entire hardware needed */
{
	int fd = 0;
	
	// Counter 2, mode 4, filled with 0xFFFF
	sysOutByte (aioBase + cntCntrlReg, cnt2Sel + bothByteSel + cntMode4);
	sysOutByte (aioBase + cnt2Address, 0xFF);
	sysOutByte (aioBase + cnt2Address, 0xFF);

	// Digital IOs:
	if (board == 0)
		/* Digital IOs: A input, B output, C half input, half output */
		sysOutByte (aioBase + ioCntrlReg, 0x91);
	else if (board == 1) {
		/* Digital IOs: A input, B output, C entirely input */
		sysOutByte (aioBase + ioCntrlReg, 0x99);
		
		/* Initalize second serial interface */
		fd = open ("/tyCo/1", O_RDWR, 0);
		ioctl (fd, FIOBAUDRATE, 115200);

		/* Start connection to PIC-Board */
		write (1, "rson\r\n", 6); /* for debug */
		write (fd, "rson\r\n", 6);
	}

	// Display:
	dispHrdInit ();
	dispCharPos (&dispConDv,0);
	dispClear (&dispConDv,0,' ');
	dispCursorOff();
	sysOutByte (aioBase + ioRegC, displayRelayBitOn);

	// D/A converter:
	sysOutByte (aioBase + dacRefEnReg, 0x01);
	
	return fd;
}

void writeToDisplay (UCHAR row, UCHAR column, char* stringBuffer)
{
	dispCharPos (&dispConDv,(column + 80 * row));
	dispWriteString (&dispConDv, stringBuffer);
	return;
}

UCHAR readDigital (UCHAR port, UCHAR bit) {
	/*	port:	0x10 (PORTA)
				0x11 (PORTB)
				0x12 (PORTC)
		bit:	0 ... 7 depending on position
		return value is 0 or 1 (on the lowest bit, the other are 0)
	*/
	UCHAR temp;

	temp = sysInByte(aioBase + port);
	temp = temp >> bit;
	temp = temp & 0x01; /* mask lowest bit only */

	return (temp);
}

void writeDigital (UCHAR port, UCHAR bit, UCHAR value) {
	/*	port:	0x10 (PORTA)
				0x11 (PORTB)
				0x12 (PORTC)
		bit:	0 ... 7 depending on position
		value is 0 or 1 (on the lowest bit, the other must be 0)
	*/
	UCHAR temp, temp2;

	temp = value & 0x01;
	if (value == 0x01) {
		temp = value << bit;
		temp2 = sysInByte (aioBase + port);
		temp = temp | temp2;
		sysOutByte(aioBase + port, temp);
	} 
	else {
		temp = 0x01;
		temp = temp << bit;
		temp = ~ temp;
		temp2 = sysInByte (aioBase + port);
		temp = temp & temp2;
		sysOutByte(aioBase + port, temp);
	}
	return;
}

int readAnalog (UCHAR input, UCHAR range)
/*	input = 0 … 7
	range:	0 (0 … +5V)
			1 (-5V … +5V)
			2 (0 … +10V)
			3 (-10V … +10V)
	return value is 0 … 4095
*/
{
	UCHAR temp;

	switch (range)
	{
		case 0: temp = 0;
				break;
		case 1: temp = 0x8;
				break;
		case 2: temp = 0x10;
				break;
		case 3: temp = 0x18;
				break;
		default:
				break;
	}
	sysOutByte (aioBase + adcCntrlReg, temp + input);
	do
	{
		temp = sysInByte (aioBase + adcStatusReg);
		temp = temp & 0x80;
	}
	while (temp == 0);
	return (sysInWord (aioBase + adcDataReg));
}

void writeAnalog (UCHAR output, int value)
/*	output = 0 … 3
	value = 0 … 4095
*/
{
	switch (output)
		{
		case 0: sysOutWord (aioBase + 0x04, value);
				break;
		case 1: sysOutWord (aioBase + 0x06, value);
				break;
		case 2: sysOutWord (aioBase + 0x08, value);
				break;
		case 3: sysOutWord (aioBase + 0x0A, value);
				break;
		default:
				break;
	}
	return;
}

int getEncoderPulses (void)
/*	returns encoder pulses since last call
	WARNING: when it is called the first time, it will return nonsense!
*/
{
	unsigned short temp;
	/* latch the counter in order to read it */
	sysOutByte (aioBase + cntCntrlReg, cnt2Sel + latchSel);

	/* read the counter value and calculate rotation speed */
	temp = sysInByte (aioBase + cnt2Address);
	temp = temp + 256 * sysInByte (aioBase + cnt2Address);

	/* correct because the counter needs a clock pulse to load value first */
	temp--;
	
	/* Calculate number of pulses */
	temp = 0xFFFF - temp;

	/* set the counter 2 to mode 4 and fill it with 0xFFFF high byte first */
	sysOutByte (aioBase + cntCntrlReg, cnt2Sel + bothByteSel + cntMode4);
	sysOutByte (aioBase + cnt2Address, 0xFF);
	sysOutByte (aioBase + cnt2Address, 0xFF);
	return(temp);
}

int getEncoderPulsesZeroCorrected (void)
/*	returns encoder pulses since last call
	WARNING: when it is called the first time, it will return nonsense!
*/
{
	unsigned short temp;
	/* latch the counter in order to read it */
	sysOutByte (aioBase + cntCntrlReg, cnt2Sel + latchSel);

	/* read the counter value and calculate rotation speed */
	temp = sysInByte (aioBase + cnt2Address);
	temp = temp + 256 * sysInByte (aioBase + cnt2Address);

	/* pulsCnt == 1 means last time a 0xFFFF has been written */
	if (pulsCnt == 1) { 
		/* The value written last time has not been loaded, means no pulse */
		if (temp < 0x8000){
			temp = 0;
			pulsCnt = 0;
		}
		else {
			/* correct because the counter needs a clock pulse to load value first */
			temp--;
			/* Calculate number of pulses */
			temp = 0xFFFF - temp;
		}
	}
	
	/* pulsCnt == 0 means last time a 0x7FFF has been written */
	else { 
		/* The value written last time has not been loaded, means no pulse */
		if (temp > 0x7FFF){
			temp = 0;
			pulsCnt = 1;
		}
		else {
			/* correct because the counter needs a clock pulse to load value first */
			temp--;
			/* Calculate number of pulses */
			temp = 0x7FFF - temp;
		}
	}

	/* set the counter 2 to mode 4 and fill it with 0xFFFF or 0x7FFF low byte first */
	sysOutByte (aioBase + cntCntrlReg, cnt2Sel + bothByteSel + cntMode4);
	sysOutByte (aioBase + cnt2Address, 0xFF);
	if (pulsCnt == 0) {
		sysOutByte (aioBase + cnt2Address, 0xFF);
		pulsCnt = 1;
	}
	else {
		sysOutByte (aioBase + cnt2Address, 0x7F);
		pulsCnt = 0;
	}
	return(temp);
}

int getRotationDirection (void)
/* left = -1, right = +1 */
{
	UCHAR temp;

	temp = sysInByte (aioBase + encoderDirAddress);
	temp = temp & encoderDirBit;
		if (temp == 0) return (+1);
		else return (-1);
}

void motorOn (void)
{
	UCHAR temp;
	
	temp = sysInByte(aioBase + motorRelayAddress) & motorRelayBitOn;
	sysOutByte (aioBase + motorRelayAddress, temp);
	return;
}

void motorOff (void)
{
	UCHAR temp;
	
	temp = sysInByte(aioBase + motorRelayAddress) | motorRelayBitOff;
	sysOutByte (aioBase + motorRelayAddress, temp);
	return;
}

void watchdogSet (UCHAR time)
/* time given in seconds (1 ... 255) */
{
		sysOutByte(0x443,time);
}

void watchdogTrigger (void)
{
	sysInByte(0x443);
}

void watchdogDisable (void)
{
	sysInByte (0x43);
	sysInByte (0x843);
}
