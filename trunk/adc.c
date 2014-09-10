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
	ACSR |= 1 << ACD;

	// 1.1V als Referenz, Kanal 5 auswählen
	ADMUX |= (1 << REFS1 | 1 << REFS0 | 1 << MUX2 | 1 << MUX0);

	// ADC-Vorteiler = 64 (9.8304MHz / 64 = 153kHz)
	ADCSRA |= (1 << ADPS2 | 1 << ADPS1);

	// ADC einschalten und Dummy-Wandlung durchführen
	ADCSRA |= 1 << ADEN;
	ADCSRA |= 1 << ADSC;

	// Auf Ende der Dummy-Wandlung warten
	while (ADCSRA & (1 << ADSC))
		;
	nosense = ADCW;
}

// Ausführen von 11 Messungen, Mittelwertbildung,
// Rückrechnung auf Spannungswert des Versorgers
uint8_t adc_read(uint8_t channel) {
	uint32_t result = 0;

	ADMUX &= ~(0x07);
	ADMUX |= channel;

	// 11 Messungen, Summenbildung
	for (uint8_t i = 0; i < 11; i++) {
		ADCSRA |= 1 << ADSC;
		while (ADCSRA & (1 << ADSC))
			;
		result += ADCW;
	}
	// Mittelwertbildung durch Division durch 8 mit Rundung
	// Umrechnung des Mittelwerts in Spannungswert (in Dezivolt) durch:
	// U =    160/10     *   10     *    MW      * 1.1 / 1024
	//    Spannungsteiler   Volt ->       ADC-      Umrechnung: Wert 1024
	//      100k + 1500k     Dezivolt   Mittelwert   entspricht 1.1 Volt
	// Kürzen U = 16*11*MW/1024 = 11*MW/64 und runden
	result = (result + 32) >> 6;
	return (uint8_t) result;
}
