/*	Author: Taeho Yoo
 *  Partner(s) Name: 
 *	Lab Section: 23
 *	Assignment: Lab #7  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *	Demo Link:  https://www.youtube.com/watch?v=7-899qwU7A4&feature=youtu.be
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() { TCCR1B = 0x00; }
void TimerISR() { TimerFlag = 1; }

ISR(TIMER1_COMPA_vect) { 
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States {Start, One, Two, Three, Four, Press,  Release}  state;
unsigned char pressed = 0;
unsigned char score = 5;

void Tick() {
	switch(state) {
			case Start:
				PORTB = 0x00;
				state = One;
				LCD_Cursor(1);
				LCD_WriteData(score + '0');
				break;
			case One:
				if((~PINA & 0x01) == 1 && pressed == 0) {
					state = Press;
					if(score > 0) { 
						score--;
						LCD_Cursor(1);
						LCD_WriteData(score + '0');

				}
				else if((~PINA & 0x01) == 1 && pressed == 1) {
					state = Two;
			       	}
				else {
					state = Two;
					pressed = 0;
				}
				break;
			case Two:
				if((~PINA & 0x01) == 1 && pressed == 0) {
					state = Press;
					if(score < 9) {
						score++;
						LCD_Cursor(1);
						LCD_WriteData(score + '0');
					}
					else { 
						LCD_DisplayString(1, "YOU WON!!!"); 
						score = 5; //reset score
					}

				}
				else if((~PINA & 0x01) == 1 && pressed == 1) {
					state = Three;
				}
				else {
					state = Three;
					pressed = 0;
				}
				break;
			case Three:
				if((~PINA & 0x01) == 1 && pressed == 0) {
					state = Press;
					if(score > 0) {
						score--;
						LCD_Cursor(1);
						LCD_WriteData(score + '0');

				}
				else if((~PINA & 0x01) == 1 && pressed == 1)
			      	{
					state = Four;
			       	}
				else {
					state = Four;
					pressed = 0;
				}
				break;
			case Four:
				if((~PINA & 0x01) == 1 && pressed == 0) {
					state = Press;
					if(score < 9) {
						score++;
						LCD_Cursor(1);
						LCD_WriteData(score + '0');
					}
					else {
						LCD_DisplayString(1, "YOU WON!!!");
						score = 5;
					}
				}
				else if((~PINA & 0x01) == 1 && pressed == 1) {
				       	state = One;
			       	}
				else {
					state = One;
					pressed = 0;
				}
				break;
			case Press:
				if((~PINA & 0x01) == 1) {
					state = Press;
				}
				else { state = Release; }
				break;
			case Release:
				if((~PINA & 0x01) == 1) {
					state = One;
					pressed = 1;
				}
				else { state = Release; }
				break;
			default:
				break;
		}
		switch(state) {
			case One:
				PORTB = 0x01;
				break;
			case Two:
				PORTB = 0x02;
				break;
			case Three:
				PORTB = 0x04;
				break;
			case Four:
				PORTB = 0x02;
				break;
			default:
				break;
		}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF;
	PORTB = 0x00;
       	DDRA = 0x00;
	PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	LCD_init();

	TimerSet(300);
	TimerOn();
		
	while(1) {
		Tick();		
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
