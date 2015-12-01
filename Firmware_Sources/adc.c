/*
 * adc.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// De-Initialisation
void adc_deinit(void) {
	ADMUX  = 0;
	ADCSRA = 0;
}

// Initialisation
void adc_init(void) {
	uint16_t nosense;

	// Comparator off
	ACSR |= 1 << ACD;

	// VCC as reference, select channel 5
	ADMUX |= (1 << REFS0 | 1 << MUX2 | 1 << MUX0);

	// ADC-Prescaler = 64 (9.8304MHz / 64 = 153kHz)
	ADCSRA |= (1 << ADPS2 | 1 << ADPS1);

	// Turn on ADC and execute dummy-conversion
	ADCSRA |= 1 << ADEN;
	ADCSRA |= 1 << ADSC;

	// Wait for end of dummy conversion
	while (ADCSRA & (1 << ADSC)) ;

	nosense = ADCW;

	_delay_ms(250);

	// Check if correct reference is selected
	nosense = 0;

	for (uint8_t i = 8; i; i--) {
		ADCSRA |= 1 << ADSC;

		while (ADCSRA & (1 << ADSC)) ;

		nosense += ADCW;
	}

	nosense >>= 3;

	// Switch reference if "wrong" reference selection is detected
	if (nosense < 400) {
		ADMUX |= (1 << REFS1);

		_delay_ms(250);

		ADCSRA |= 1 << ADSC;

		while (ADCSRA & (1 << ADSC)) ;

		nosense = ADCW;
	}
}

// Calculation of battery voltage
uint8_t adc_read(uint8_t channel) {
	uint32_t result = 0;
	uint8_t  imax   = 10;

	ADMUX &= ~(0x07);
	ADMUX |= channel;

	if (ADMUX & (1 << REFS1)) imax = 11;

	// 10 or 11 measurements (reference-dependent), summing up
	for (uint8_t i = imax; i; i--) {
		ADCSRA |= 1 << ADSC;

		while (ADCSRA & (1 << ADSC)) ;

		result += ADCW;
	}

	// Transform ADC-value to voltage value (in dezivolt) by:
	// 1.) U =    16          *   10     *    MW      * 1.1 / 1024
	//    Voltage divider   Volt ->       ADC-      Transform: 1024
	//      100k + 1500k    Dezivolt      Mean      equals 1.1 Volt
	// Kürzen U = 16*11*MW/1024 = 11*MW/64 und runden
	//
	// 2.) U =    3.2         *   10     *    MW      * 5 / 1024
	//    Voltage divider   Volt ->       ADC-     Transform: 1024
	//      100k + 220k     Dezivolt      Mean     equals 5 Volt
	// Kürzen U = 32*5*MW/1024 = 10*MW/64 und runden
	//
	// Difference between case 1 and 2 (pre-factor 11 or 10) is already taken
	// into account during measurements
	//
	result = (result + 32) >> 6;

	return (uint8_t)result;
}
