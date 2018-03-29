/*
 * ButtonFlashACX.c
 *
 * Created: 3/27/2018 2:33:37 PM
 * Author : cookab1
 */ 

#include <avr/io.h>
#include "acx.h"
#include "acxserial.h"

#define LISTENER 0
#define OFF 0
#define ROTATE 1
#define FLASH 2

void buttonListener();
void rotateThread();
void flashThread();

void Serial_puts(uint8_t, char *);

int stateTable[2][3] = {{ROTATE,OFF,ROTATE},
						{FLASH,FLASH,OFF}};
volatile int state = OFF;

int main(void)
{
	DDRK = 0x0f; // bits 0-3 are output
	PORTK = 0x00;  // clear to zero
	DDRF = 0x0; // bits 0-1 are input from buttons
	PORTF = 0x03; // input bits set to 1 as the start state
	
	// after calling x_init(), the running thread is "thread 0"
	x_init();
	x_new(LISTENER, buttonListener, true);
	x_new(ROTATE, rotateThread, true);
	x_new(FLASH, flashThread, true);
	
	// We are thread 0 now
	DDRB = 0x80;
	
	/* Replace with your application code */
	while (1)
	{	
		switch(state){
			case OFF:
				x_suspend(ROTATE);
				x_suspend(FLASH);
				break;
			case ROTATE:
				x_suspend(FLASH);
				x_resume(ROTATE);
				break;
			case FLASH:
				x_suspend(ROTATE);
				x_resume(FLASH);
				break;
		}
	}
}

void buttonListener() {
	int button;
	while(1)
	{
		// run thread main
		//listen for button press
		   //while(button not released);
		while(((PINF & 1) != 0) || ((PINF & 2) != 0));
		button = (PINF & 1) == 0 ? 0 : 1;
		
		if(button) {
			while((PINF & 2) != 1);
			//button 2 released
		} else {
			while((PINF & 1) != 1);
			//button 1 released
		}
		state = stateTable[button][state];
	}
}

void rotateThread() {
	DDRK |= 0x02;
	while(1)
	{		
		// run thread main
		PORTK |= 0x1;
		x_delay(1000);
		PORTK &= 0xf0;
		
		PORTK |= 0x2;
		x_delay(1000);
		PORTK &= 0xf0;
		
		PORTK |= 0x4;
		x_delay(1000);
		PORTK &= 0xf0;
		
		PORTK |= 0x8;
		x_delay(1000);
		PORTK &= 0xf0;
	}
}
void flashThread() {
	DDRK |= 0x04;
	while(1)
	{		
		// run thread main
		PORTK |= 0x0f;  //turn lights on
		x_delay(200);	
		PORTK &= 0xf0;  //turn lights off
		x_delay(200);	
	}
}
