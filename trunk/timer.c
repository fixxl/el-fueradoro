/*
 * timer.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// Timer aktivieren (Vorteiler 256) => Overflow nach 1,7 Sekunden
void timer_on(void) {
	TCCR1B	|=	(1<<CS12);
}

// Timer ausschalten
void timer_off(void) {
	TCCR1B	&= ~(1<<CS12 | 1<<CS11 | 1<<CS10);
}

// Timer zurücksetzen (Zählerstand auf 0, Überlauf-Flag löschen)
inline void timer_reset(void) {
	TIFR1 = 1<<TOV1;
	TCNT1 = 0;
}


