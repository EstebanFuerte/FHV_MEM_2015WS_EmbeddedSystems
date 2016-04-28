/* kbd.c - Pilsan Keyboard C File */

/*
DESCRIPTION
This is a file that reads the keyboard and returns the key pressed.

INCLUDE FILES
  - kbd.h: keyboard hardware defines
 
*/

/* includes */


#include "kbd.h"

#include <stdio.h>
#include <sysLib.h>

char getKey (void)
{
	char tempKey, tempIn, tempOut;
/* Keyboard In
 * reads kbd in by setting outs one by one */
	tempKey = 0;

/* First out bit */
	tempIn = sysInByte (aioBase + kbdOutAddress);
	tempIn = tempIn & kbdOutMask;
	tempOut = tempIn | kbdOut0Bit;
	sysOutByte (aioBase + kbdOutAddress, tempOut); 
	tempIn = sysInByte (aioBase + kbdInAddress);
	tempIn = tempIn & kbdInMask;
	switch (tempIn)
	{
		case 0x70:
			tempKey = 'F';
			break;
		case 0xB0:
			tempKey = '3';
			break;
		case 0xD0:
			tempKey = '2';
			break;
		case 0xE0:
			tempKey = '1';
			break;
	}

/* Second out bit */
	tempIn = sysInByte (aioBase + kbdOutAddress);
	tempIn = tempIn & kbdOutMask;
	tempOut = tempIn | kbdOut1Bit;
	sysOutByte (aioBase + kbdOutAddress, tempOut); 
	tempIn = sysInByte (aioBase + kbdInAddress);
	tempIn = tempIn & kbdInMask;
	switch (tempIn)
	{
		case 0x70:
			tempKey = 'E';
			break;
		case 0xB0:
			tempKey = '6';
			break;
		case 0xD0:
			tempKey = '5';
			break;
		case 0xE0:
			tempKey = '4';
			break;
	}
		
/* Third out bit */
	tempIn = sysInByte (aioBase + kbdOutAddress);
	tempIn = tempIn & kbdOutMask;
	tempOut = tempIn | kbdOut2Bit;
	sysOutByte (aioBase + kbdOutAddress, tempOut); 
	tempIn = sysInByte (aioBase + kbdInAddress);
	tempIn = tempIn & kbdInMask;
	switch (tempIn)
	{
		case 0x70:
			tempKey = 'D';
			break;
		case 0xB0:
			tempKey = '9';
			break;
		case 0xD0:
			tempKey = '8';
			break;
		case 0xE0:
			tempKey = '7';
			break;
	}
	
/* Fourth out bit */
	tempIn = sysInByte (aioBase + kbdOutAddress);
	tempIn = tempIn & kbdOutMask;
	tempOut = tempIn | kbdOut3Bit;
	sysOutByte (aioBase + kbdOutAddress, tempOut); 
	tempIn = sysInByte (aioBase + kbdInAddress);
	tempIn = tempIn & kbdInMask;
	switch (tempIn)
	{
		case 0x70:
			tempKey = 'C';
			break;
		case 0xB0:
			tempKey = 'B';
			break;
		case 0xD0:
			tempKey = '0';
			break;
		case 0xE0:
			tempKey = 'A';
			break;
	}
	return (tempKey);
}
;