/*
 * shiftregister.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// Initialisierung
void sr_init(void) {
	// Pullup an /OE einschalten, damit /OE beim Umschalten von Eingang auf Ausgang
	// "high" bleibt und die Ausg�nge weiter inaktiv sind!
	OE_PORT		|=  (1<<OE);

	// Pins als Ausg�nge konfigurieren
	SER_IN_DDR 	|= 1<<SER_IN;
	OE_DDR	 	|= 1<<OE;
	RCLOCK_DDR	|= 1<<RCLOCK;
	SCLOCK_DDR	|= 1<<SCLOCK;

	// Alle Ausg�nge au�er /OE auf low
	SER_IN_PORT &= ~(1<<SER_IN);
	RCLOCK_PORT	&= ~(1<<RCLOCK);
	SCLOCK_PORT	&= ~(1<<SCLOCK);

	// Alle Schieberegisterpositionen 0 setzen...
	for(uint8_t j=0; j<3; j++) {
		for(uint8_t i=0; i<(SR_CHANNELS+4); i++) {
			SCLOCK_PIN 	=  (1<<SCLOCK);			// Pin durch Toggling high
			SCLOCK_PIN 	=  (1<<SCLOCK);			// Pin durch Toggling low
		}
		// ... und ins Ausgaberegister �bernehmen.
		RCLOCK_PIN 	=  (1<<RCLOCK);				// Pin durch Toggling high
		RCLOCK_PIN 	=  (1<<RCLOCK);				// Pin durch Toggling low
	}
	_delay_ms(50);

	// Ausg�nge k�nnen jetzt gefahrlos aktiviert werden!
	OE_PORT &= ~(1<<OE);
}


// Bitmuster an Ausg�ngen darstellen
void sr_shiftout(uint16_t scheme) {
	uint8_t i;
	uint16_t mask = 1<<(SR_CHANNELS-1);

	SER_IN_PORT &= ~(1<<SER_IN);
	SCLOCK_PORT &= ~(1<<SCLOCK);
	RCLOCK_PORT &= ~(1<<RCLOCK);

	for(i=SR_CHANNELS; i>0; i--) {
		if (scheme & mask)	{
			SER_IN_PORT |=   1<<SER_IN;
		}
		SCLOCK_PIN  =  (1<<SCLOCK);				// Pin durch Toggling high
		SCLOCK_PIN  =  (1<<SCLOCK);				// Pin durch Toggling low
		SER_IN_PORT &= ~(1<<SER_IN);
		mask >>= 1;
	}
	RCLOCK_PIN	=  (1<<RCLOCK);					// Pin durch Toggling high
	RCLOCK_PIN	=  (1<<RCLOCK);					// Pin durch Toggling low
}

