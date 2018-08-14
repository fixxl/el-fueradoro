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
    ACSR     |= 1 << ACD;

    // VCC as reference, select channel 5
    ADMUX    |= (1 << REFS1 | 1 << REFS0 | 1 << MUX2 | 1 << MUX0);

    // ADC-Prescaler = 64 (9.8304MHz / 64 = 153kHz)
    ADCSRA   |= (1 << ADPS2 | 1 << ADPS1);

    // Turn on ADC and execute dummy-conversion
    ADCSRA   |= 1 << ADEN;
    ADCSRA   |= 1 << ADSC;

    // Wait for end of dummy conversion
    while (ADCSRA & (1 << ADSC));

    nosense   = ADCW;

    _delay_ms(250);

    // Check if correct reference is selected
    nosense   = 0;

    for (uint8_t i = 8; i; i--) {
        ADCSRA  |= 1 << ADSC;

        while (ADCSRA & (1 << ADSC));

        nosense += ADCW;
    }

    nosense >>= 3;

    // Switch reference if "wrong" reference selection is detected
    if (nosense < 400) {
        ADMUX  |= (1 << REFS1);

        _delay_ms(250);

        ADCSRA |= 1 << ADSC;

        while (ADCSRA & (1 << ADSC));

        nosense = ADCW;
    }
}

static uint16_t adc_read(uint8_t channel) {
    uint16_t result = 0;

    ADMUX   &= ~(0x07);
    ADMUX   |= channel;

    // 8 measurements (reference-dependent), summing up
    for (uint8_t i = 8; i; i--) {
        ADCSRA |= 1 << ADSC;

        while (ADCSRA & (1 << ADSC));

        result += ADCW;
    }
    result >>= 3;

    return result;
}

// Calculation of battery voltage
uint8_t bat_calc(uint8_t channel) {

	uint8_t result;
	uint16_t voltage_raw = adc_read(channel);

    // voltage_raw * 11 / 64 = voltage_in
    result = (11 * voltage_raw + 32) >> 6;

    return result;
}

uint8_t imp_calc(uint8_t channel) {
    uint16_t voltage_raw;
    uint8_t result;

    for(uint8_t i = 0; i < 16; i++) {
    	voltage_raw = adc_read(channel);

    	// 25/512 * voltage_raw = R
    	result = (25 * voltage_raw + 256) >> 9;
    }
    return result;
}
