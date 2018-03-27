/*
 * ButtonFlashACX.c
 *
 * Created: 3/27/2018 2:33:37 PM
 * Author : cookab1
 */ 

#include <avr/io.h>
#include "acx.h"
#include "acxserial.h"

void buttonListener();
void rotateThread();
void flashThread();
//void offThread();

void Serial_puts(uint8_t, char *);

int stateTable[2][3] = {{1,0,1},
						{2,2,0}};
volatile int state = 0;

int main(void)
{
	DDRK = 0x0f; // bits 0-3 are output
	PORTK = 0x00;  // clear to zero
	DDRF = 0x0; // bits 0-1 are input from buttons
	PORTF = 0x03; // input bits set to 1 as the start state
	
	// after calling x_init(), the running thread is "thread 0"
	x_init();
	x_new(0, buttonListener, true);
	x_new(1, rotateThread, true);
	x_new(2, flashThread, true);
	
	// We are thread 0 now
	DDRB = 0x80;
	
	/* Replace with your application code */
	while (1)
	{	
		switch(state){
			case 0:
				x_suspend(1);
				x_suspend(2);
				break;
			case 1:
				x_suspend(2);
				x_resume(1);
				break;
			case 2:
				x_suspend(1);
				x_resume(2);
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
		if((PINF & 1) == 0)
			button = 0;
		else
			button = 1;
		while(((PINF & 1) != 1) || ((PINF & 2) != 1));
		   //change state; state = stateTable[button][state];
	}
}

void rotateThread() {
	DDRK |= 0x02;
	while(1)
	{		
		// run thread main
		PORTK ^= 0x02;
		x_delay(200);
	}
}
void flashThread() {
	DDRK |= 0x04;
	while(1)
	{		
		// run thread main
		PORTF |= 0x0f;  //turn lights on
		x_delay(200);		
		PORTF &= 0xf0;  //turn lights off
	}
}
