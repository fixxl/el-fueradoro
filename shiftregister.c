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
	// "high" bleibt und die Ausgänge weiter inaktiv sind!
	OE_PORT		|=  (1<<OE);

	// Pins als Ausgänge konfigurieren
	SER_IN_DDR 	|= 1<<SER_IN;
	OE_DDR	 	|= 1<<OE;
	RCLOCK_DDR	|= 1<<RCLOCK;
	SCLOCK_DDR	|= 1<<SCLOCK;

	// Alle Ausgänge außer /OE auf low
	SER_IN_PORT &= ~(1<<SER_IN);
	RCLOCK_PORT	&= ~(1<<RCLOCK);
	SCLOCK_PORT	&= ~(1<<SCLOCK);

	// Alle Schieberegisterpositionen 0 setzen...
	for(uint8_t j=0; j<3; j++) {
		for(uint8_t i=0; i<(SR_CHANNELS+4); i++) {
			SCLOCK_PIN 	|=  (1<<SCLOCK);
			SCLOCK_PIN 	|=  (1<<SCLOCK);
		}
		// ... und ins Ausgaberegister übernehmen.
		RCLOCK_PIN 	|=  (1<<RCLOCK);
		RCLOCK_PIN 	|=  (1<<RCLOCK);
	}
	_delay_ms(50);

	// Ausgänge können jetzt gefahrlos aktiviert werden!
	OE_PORT &= ~(1<<OE);
}


// Bitmuster an Ausgängen darstellen
void sr_shiftout(uint16_t scheme) {
	uint8_t i;
	uint16_t mask = 1<<(SR_CHANNELS-1);

	RCLOCK_PORT &= ~(1<<RCLOCK);

	for(i=SR_CHANNELS; i>0; i--) {
		if (scheme & mask)	SER_IN_PORT |=   1<<SER_IN;
		SCLOCK_PIN  |=  (1<<SCLOCK);
		SCLOCK_PIN  |=  (1<<SCLOCK);
		SER_IN_PORT &= ~(1<<SER_IN);
		mask >>= 1;
	}
	RCLOCK_PIN	|=  (1<<RCLOCK);
	RCLOCK_PIN	|=  (1<<RCLOCK);
}

