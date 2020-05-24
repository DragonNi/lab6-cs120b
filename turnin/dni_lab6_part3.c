/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #3
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

unsigned int count = 7;
unsigned int hold = 0;
enum States{start, store, incr, incrHold, decr, decrHold, reset} state;

void Tick(){
	unsigned char tempA = ~PINA;
	switch(state){
		case start:
			state = store;
			break;
		case store:
			if((tempA & 0x03) == 0x01){
				state = incr;
			}
			else if((tempA & 0x03) == 0x02){
				state = decr;
			}
			else if((tempA & 0x03) == 0x03){
				state = reset;
			}
			else{
				state = store;
			}
			break;
		case incr:
			if((tempA & 0x03) == 0x01){
				state = incrHold;
			}
			else if((tempA & 0x03) == 0x02){
				state = decr;
			}
			else if((tempA & 0x03) == 0x03){
				state = reset;
			}
			else{
				state = store;
			}
			break;
		case incrHold:
			if((tempA & 0x03) == 0x01 && hold < 10){
				state = incrHold;
			}
			else if((tempA & 0x03) == 0x01 && hold == 10){
				hold = 0;
				state = incr;
			}
			else if((tempA & 0x03) == 0x02){
				hold = 0;
				state = decr;
			}
			else if((tempA & 0x03) == 0x03){
				hold = 0;
				state = reset;
			}
			else{
				hold = 0;
				state = store;
			}
			break;

		case decr:
			if((tempA & 0x03) == 0x01){
				state = incr;
			}
			else if((tempA & 0x03) == 0x02){
				state = decrHold;
			}
			else if((tempA & 0x03) == 0x03){
				state = reset;
			}
			else{
				state = store;
			}
			break;
		case decrHold:
			if((tempA & 0x03) == 0x02 && hold < 10){
				state = decrHold;
			}
			else if((tempA & 0x03) == 0x02 && hold == 10){
				hold = 0;
				state = decr;
			}
			else if((tempA & 0x03) == 0x01){
				hold = 0;
				state = incr;
			}
			else if((tempA & 0x03) == 0x03){
				hold = 0;
				state = reset;
			}
			else{
				hold = 0;
				state = store;
			}

			break;

		case reset:
			if((tempA & 0x03) == 0x03){
				state = reset;
			}
			else if((tempA & 0x03) == 0x01){
				state = incr;
			}
			else if((tempA & 0x03) == 0x02){
				state = decr;
			}
			else{
				state = store;
			}
			break;

		default:
			state = start;
			break;
	}

	switch(state){
		case start:
			break;

		case store:
			PORTB = count;
			break;

		case incr:
			if(count != 9){
				count++;
			}
			PORTB = count;
			break;

		case incrHold:
			hold++;
			break;

		case decr:
			if(count != 0){
				count--;
			}
			PORTB = count;
			break;

		case decrHold:
			hold++;
			break;

		case reset:
			count = 0;
			PORTB = count;
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
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(100);
    TimerOn();

    while (1) {
	Tick();
	while(!TimerFlag);

	TimerFlag = 0;
    }
    return 1;
}
