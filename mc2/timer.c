/*
 * timer.c
 *
 *  Created on: Dec 18, 2020
 *      Author: Dell
 */

#include "timer.h"


void TIMER1_CTC_MODE (void)
{
	cli();
	TCNT1 = 0;
	OCR1A = 960;

	TCCR1A |= (1<<FOC1A) | (1<<FOC1B);
	TCCR1B |= (1<<CS12) | (1<<CS10) | (1<<WGM12);

	TIMSK |= (1<<OCIE1A);
	sei();
}

void Stop_Timer1 (void)
{
	TCCR1B &=~ (1<<CS10) ;
	TCCR1B &=~ (1<<CS11) ;
	TCCR1B &=~ (1<<CS12) ;
	TCNT1 = 0;

}


