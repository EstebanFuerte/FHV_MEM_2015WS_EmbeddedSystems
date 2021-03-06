/* MEM.c - Pilsan Timer Example Program */

/*
DESCRIPTION
This is a file containing timer and interrupt for use with RTOS
*/

#include "hwFunc.h"
#include "disp.h"
#include "FHV.h"
#include "hw.h"
#include "time.h"

#include <stdlib.h>
#include <stdio.h>
#include <intLib.h>
#include <taskLib.h>
#include <sysLib.h>

SEM_ID timIntSem = NULL;
SEM_ID writeSem = NULL;

int AInPot;
int AInTemp;
int key = 49;
int count;

void tRead (void);
void tWrite(void);
void tKeyboard(void);
void tTime(void);

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
	
	// create Semaphores
	timIntSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	writeSem = semBCreate(SEM_Q_PRIORITY, SEM_FULL); //SEM_FULL = Stift ist da
	
	int readTask;
	readTask = taskSpawn("tRead", 101, 0, 0x1000, (FUNCPTR) tRead,0,0,0,0,0,0,0,0,0,0);
	
	/* Lab 01 - The third task shall do the following:
	 *	- It writes the values of the global variables (AIn) onto the display.
	 *	- This shall happen approx. each 100 ms
	 */
	int writeTask;
	writeTask = taskSpawn("tWrite",102,0,0x1000, (FUNCPTR) tWrite,0,0,0,0,0,0,0,0,0,0);
	
	/* Lab02 - The fourth task shall do the following:
	* - It reads the keyboard approx. each 100 ms.
	* - If a new key has been pressed, it is written into a global variable (Key).
	*/
	int keyboardTask;
	keyboardTask = taskSpawn("tKeyboard", 103, 0,0x1000, (FUNCPTR) tKeyboard,0,0,0,0,0,0,0,0,0,0);
	
	/* Lab 02 - Task for show time on display*/
	int timeTask;
	timeTask = taskSpawn("tTime", 103, 0,0x1000, (FUNCPTR) tTime,0,0,0,0,0,0,0,0,0,0);
	
	/* Suspend own task */
	taskSuspend (0);	//suspended sich selber
} /* main */

// Lab01 - for second task
void tRead (void) {
	
	while (1) {
		semTake(timIntSem, WAIT_FOREVER);
		
		
		int newPot = readAnalog(2,0);
		int diffPot = newPot-AInPot;
		//printf("AInPot:%d; NewPot: %d; DiffPot: %d\n", AInPot, newPot,diffPot);
		if ((diffPot >20) || (diffPot <(-20))) {
			AInPot = newPot;
			//printf("new potValue: %d\n",AInPot);
		}
		
		int newTemp = readAnalog(5,2);
		int diffTemp = AInTemp - newTemp;
		if((diffTemp >10) || (diffTemp <(-10))){
			AInTemp = newTemp;
			//printf("new tempValue: %d\n",AInTemp);
		}
	}
}

// Lab01/2 -  for third task
void tWrite(void){
	while(1){
		beLazy(100);
		char stringBufferPot[50];
		char stringBufferTemp[50];
		char stringBufferKey[50];
		
		semTake(writeSem, WAIT_FOREVER);
		sprintf(stringBufferTemp, "Temperature: %d   ",AInTemp);
		//sprintf(stringBufferPot, "Potentiomenter: %d   ",AInPot);
		sprintf(stringBufferKey, "Gedrueckte Taste: %d   ", key);
		
		if(key == 49){
			sprintf(stringBufferPot, "Potentiometer: %d                      ",AInPot);
		}
		else if(key == 50){
			//calculation of the voltage based on the input value 58...4095
			float k = (5.0/4037);
			float d = (float)(-k*58);
			float calc = (float)(k* AInPot+d);
			sprintf(stringBufferPot, "Potentiometer: %f V                      ",calc);
		}
		else if (key == 51){
			sprintf(stringBufferPot, "Potentiometer: %x                        ", AInPot);
		}
		else{
			sprintf(stringBufferPot, "Error - key 1,2 or 3 should be used");
		}
		writeToDisplay(0,0, stringBufferPot);
		writeToDisplay(1,0, stringBufferTemp);
		writeToDisplay(2,0, stringBufferKey);
		semGive(writeSem);
	}
}

// Lab02 
void tKeyboard(void){
	while(1){
		beLazy(100);
		//printf("Taste %d gedr�ckt \n",getKey());
		int tempKey = getKey();
		if(tempKey != 0){
			key = tempKey;
		}
	}
	
}

void tTime(void){
	while(1){
		
		time_t timedate;
		beLazy(3000);
		count +=3;
		
		char stringBufferTime[50];
		char stringBufferCount[50];
		
		timedate = time(NULL);
		semTake(writeSem, WAIT_FOREVER);
		sprintf(stringBufferTime, "Aktuelle Zeit: %s", ctime(&timedate));
		sprintf(stringBufferCount, "Count: %d                      ",count);
		
		writeToDisplay(4,0,stringBufferCount);
		writeToDisplay(5,0,stringBufferTime);
		
		semGive(writeSem);
	}
}
