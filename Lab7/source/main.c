/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
*      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
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

unsigned char countOne = 0;
unsigned char countTwo = 0;
unsigned char counter = 0;
enum States {Start, Off, WaitUntilThree, Increment, Decrement, Reset} state;

void Tick() {
	unsigned char temp = ~PINA & 0x03;
	unsigned char tempB = 0x00;

	switch(state) {
		case Start:
			PORTB = tempB;
			state = Off;
			LCD_Cursor(1);
			LCD_WriteData('0');
			break;
		case Off:
			if(temp == 0x01) {
				state = Increment;
				if(PORTB < 0x09) {
					PORTB++;
					LCD_Cursor(1);
					LCD_WriteData(PORTB + '0');
				}
				break;
			}
			else if(temp == 0x02) {
				state = Decrement;
				if(PORTB > 0x00) {
					PORTB--;
					LCD_Cursor(1);
					LCD_WriteData(PORTB + '0');
				}
				break;
			}
			else if(temp == 0x03) {
				PORTB = 0;
				state = Reset;
				LCD_Cursor(1);
				LCD_WriteData('0');
				break;
			}
			else {
				state = Off;
				break;
			}
		case Increment:
			state = WaitUntilThree;
			break;
		case Decrement:
			state = WaitUntilThree;
			break;
		case WaitUntilThree:
			if(temp == 0x01 || temp == 0x02) {
				if(temp == 0x01) {
					if(PORTB < 9) {
						if(counter < 9) {
							counter++;
						}
						else  {
							counter = 0;
							PORTB++;
							LCD_Cursor(1);
							LCD_WriteData(PORTB + '0');
						}

					}
					state = WaitUntilThree;

				}
				else if(temp == 0x02) {
					if(PORTB > 0) {
						if(counter < 9) {
							counter++;
						}
						else {
							counter = 0;
							PORTB--;
							LCD_Cursor(1);
							LCD_WriteData(PORTB + '0');
						}

					}
					state = WaitUntilThree;
				}
			}
			else if(temp == 0x03) {
				state = Reset;
				LCD_Cursor(1);
				LCD_WriteData('0');
				PORTB = 0;
			}
			else {
				state = Off;
			}
			break;
		case Reset:
			if(temp == 0x01 || temp == 0x02) {
				state = Reset;
			}
			else {
				state = Off;
			}
			break;
		default:
			break;
	}
}
int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	LCD_init();
	TimerSet(100);
	TimerOn();

	while(1) {
		Tick();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
