/* MEM.c - Pilsan Timer Example Program */

/*
DESCRIPTION
This is a file containing timer and interrupt for use with RTOS
*/

#include "hwFunc.h"
#include "disp.h"
#include "FHV.h"
#include "hw.h"

#include <stdlib.h>
#include <stdio.h>
#include <intLib.h>
#include <taskLib.h>
#include <sysLib.h>

SEM_ID timIntSem = NULL;
int AInPot;
int AInTemp;

void tRead (void);
void tWrite(void);

void my_ISR (void)
{
	/* Add your code here! */
	
	//printf(".");
	/* ISR soll Semaphore geben */
	semGive (timIntSem);

	/* Clear AIO interrupts by writing anything to a specified address */
	sysOutByte (aioBase + intClearAddress, 0);
}

void main (void)
{
	unsigned char tempOut;
	/* Connect interrupt service routine to vector and all stuff */
	intConnect (INUM_TO_IVEC(aioIntNum), my_ISR, aioIntNum);
	sysIntEnablePIC (aioIRQNum);
	/* Enable interrupts on the aio:
	* All interrupts and interrupt from counter 1 too */
	tempOut = 0x24;
	sysOutByte (aioBase + intEnAddress, tempOut);
	
	/* Start counter 1 as timer with 50 ms period 
	* It has a clock input of 1 MHz = 1 �s 
	* Therefore the load value is 0xC350 = 50000 */
	tempOut = 0x74;
	sysOutByte (aioBase + cntCntrlReg, tempOut);
	tempOut = 0x50;
	sysOutByte (aioBase + cnt1Address, tempOut);
	tempOut = 0xC3;
	sysOutByte (aioBase + cnt1Address, tempOut);

	/* Add your code here: create tasks, semaphores, ... */
	//printf("Hello World!");
	
	/*The second task shall do the following:
	 *	- It starts counter a as an auto-reload timer with a period of 50 ms.
	 *	- The interrupt generated by this counter activates the task.
	 *		This shall be done with a semaphore.
	 *	- The task reads the analog inputs of both the potentiometer and the temperature.
	 *	- If the values have changed compared to the previous ones, these shall be written into global
	 *		variables (AIn).
	 *	- A hysteresis of e.g. 10 � 20 bit is necessary due to the noise of the analog inputs.
	 * 
	 * */
	initHardware(0);
	
	int readTask;
	readTask = taskSpawn("tRead", 101, 0, 0x1000, (FUNCPTR) tRead,0,0,0,0,0,0,0,0,0,0);
	
	timIntSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	
	/* The third task shall do the following:
	 *	- It writes the values of the global variables (AIn) onto the display.
	 *	- This shall happen approx. each 100 ms
	 */
	int writeTask;
	writeTask = taskSpawn("tWrite",102,0,0x1000, (FUNCPTR) tWrite,0,0,0,0,0,0,0,0,0,0);
	
	/* Suspend own task */
	taskSuspend (0);	//suspended sich selber
} /* main */

// for second task
void tRead (void) {
	
	while (1) {
		semTake(timIntSem, WAIT_FOREVER);
		
		
		int newPot = readAnalog(2,0);
		int diffPot = newPot-AInPot;
		printf("AInPot:%d; NewPot: %d; DiffPot: %d\n", AInPot, newPot,diffPot);
		if ((diffPot >20) || (diffPot <(-20))) {
			AInPot = newPot;
			printf("new potValue: %d\n",AInPot);
		}
		
		int newTemp = readAnalog(5,2);
		int diffTemp = AInTemp - newTemp;
		if((diffTemp >10) || (diffTemp <(-10))){
			AInTemp = newTemp;
			printf("new tempValue: %d\n",AInTemp);
		}
	}
}

// for third task
void tWrite(void){
	while(1){
		beLazy(100);
		char stringBufferPot[50];
		char stringBufferTemp[50];
		
		sprintf(stringBufferTemp, "Temperature: %d   ",AInTemp);
		sprintf(stringBufferPot, "Potentiomenter: %d   ",AInPot);
		
		writeToDisplay(0,0, stringBufferPot);
		writeToDisplay(1,0, stringBufferTemp);
	}
	
	
}
