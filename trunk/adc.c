/*
 * adc.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// Initialisierung
void adc_init(void) {
	uint16_t __attribute__ ((unused)) nosense;

	// Komparator aus
	ACSR 	|= 1<<ACD;

	// Vcc als Referenz, Kanal 5 auswählen
	ADMUX 	|= (1<<REFS0 | 1<<MUX2 | 1<<MUX0);

	// ADC-Vorteiler = 64 (9.8304MHz / 64 = 153kHz)
	ADCSRA 	|= (1<<ADPS2 | 1<<ADPS1);

	// ADC einschalten und Dummy-Wandlung durchführen
	ADCSRA 	|= 1<<ADEN;
	ADCSRA 	|= 1<<ADSC;

	// Auf Ende der Dummy-Wandlung warten
	while(ADCSRA & (1<<ADSC));
	nosense = ADCW;
}


// Ausführen von 8 Messungen, Mittelwertbildung,
// Rückrechnung auf Spannungswert des Versorgers
uint8_t adc_read(uint8_t channel) {
	uint32_t result = 0;

	ADMUX	&= ~(0x07);
	ADMUX	|= channel;

	// 8 Messungen, Summenbildung
	for(uint8_t i=0; i<8; i++) {
		ADCSRA 	|= 1<<ADSC;
		while(ADCSRA & (1<<ADSC));
		result += ADCW;
	}
	// Mittelwertbildung durch Division durch 8 mit Rundung
	// Umrechnung des Mittelwerts in Spannungswert (in Dezivolt) durch:
	// U =    320/100     *   10     *    MW      * 5 / 1024
	//    Spannungsteiler   Volt ->       ADC-      Umrechnung: Wert 1024
	//      100k + 220k     Dezivolt   Mittelwert   entspricht 5 Volt
	// Kürzen U = 160*MW/1024 = 5*MW/32 und runden
	result = (((result+4)>>3)*5 + 16)>>5;
	return (uint8_t)result;
}
