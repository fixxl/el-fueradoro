/*
 * timer.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// Activate Timer 1 (Prescaler 8) => Overflow after 53ms
void timer1_on(void) {
	TCCR1B |= (1 << CS11);
}

// Turn Timer 1 off
void timer1_off(void) {
	TCCR1B &= ~(1 << CS12 | 1 << CS11 | 1 << CS10);
}

// Reset Timer 1 (Counter to 0, clear overflow flag)
inline void timer1_reset(void) {
	TIFR1 = (1 << TOV1);
	TCNT1 = 0;
}

