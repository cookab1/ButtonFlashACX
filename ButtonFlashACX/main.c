/*
 * ButtonFlashACX.c
 *
 * Created: 3/27/2018 2:33:37 PM
 * Author : cookab1
 */ 

#include <avr/io.h>
#include "acx.h"
#include "acxserial.h"

#define LISTENER 1
#define LIGHTS 2

#define OFF 0
#define ROTATE 1
#define FLASH 2

#define ERROR 3

void buttonListener();
void rotateThread();
void flashThread();
void turnOff();

void Serial_puts(uint8_t, char *);

int stateTable[2][3] = {{ROTATE,OFF,ROTATE},
						{FLASH,FLASH,OFF}};
							
int isRelease[4][4] = {{0, 1, 2, 0}, 
					   {0, 0, 0, 2},
					   {0, 0, 0, 1},
					   {0, 0, 0, 0}};
							
volatile int buttonState = 0x3;
volatile int state = OFF;
volatile int changed = 1;

int main(void)
{
	DDRK = 0x0f; // bits 0-3 are output
	PORTK = 0x00;  // clear to zero
	DDRF = 0x0; // bits 0-1 are input from buttons
	PORTF = 0x03; // input bits set to 1 as the start state
	
	// after calling x_init(), the running thread is "thread 0"
	x_init();
	x_new(1, buttonListener, true);
	
	while (1)
	{
		if(changed) {
			switch(state){
				case OFF:
					x_disable(LIGHTS);
					turnOff();
					break;
				case ROTATE:
					x_new(LIGHTS, rotateThread, true);
					break;
				case FLASH:
					x_new(LIGHTS, flashThread, true);
					break;
			}
			changed = 0;
		}
		x_delay(5);
	}
}

void buttonListener() {
	DDRF &= 0x00;
	PORTF = 0x03;
	
	DDRB |= 0x80;
	int button;
	while(1)
	{
		int current = PINF & 3;
		if(current != buttonState) {
			if(button = isRelease[buttonState][current]) {
				state = stateTable[button - 1][state];
				changed = 1;
			}
			buttonState = current;
			
		}
		x_delay(5);
	}
}

void turnOff() {
		PORTK = 0;
}

void rotateThread() {
	DDRK |= 0x0f;
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
	DDRK |= 0x0f;
	while(1)
	{		
		// run thread main
		PORTK |= 0x0f;  //turn lights on
		x_delay(200);	
		PORTK &= 0x00;  //turn lights off
		x_delay(1000);	
	}
}