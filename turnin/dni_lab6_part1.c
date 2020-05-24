/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum States{start, l0, l1, l2} state;

void Tick(){
	switch(state){
		case start:
			state = l0;
			break;
		case l0:
			state = l1;
			break;
		case l1:
			state = l2;
			break;
		case l2:
			state = l0;
			break;

		default:
			state = start;
			break;
	}

	switch(state){
		case start:
			break;
		case l0:
			PORTB = (PORTB | 0x01) & 0x01;
			break;
		case l1:
			PORTB = (PORTB | 0x02) & 0x02;
			break;
		case l2:
			PORTB = (PORTB | 0x04) & 0x04;
			break;

		default:
			break;
	}
}

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}



int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(1000);
    TimerOn();

    /* Insert your solution below */
    while (1) {
	Tick();
	while(!TimerFlag);

	TimerFlag = 0;
    }
    return 1;
}
