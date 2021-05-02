/*
 * adc.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// De-Initialisation
void adc_deinit( void ) {
    ADMUX  = 0;
    ADCSRA = 0;
}

// Initialisation
void adc_init( void ) {
    uint16_t nosense;

    // Comparator off
    ACSR |= 1 << ACD;

    // 1.1V as reference, select channel 5
    ADMUX |= ( 1 << REFS1 | 1 << REFS0 | 1 << MUX2 | 1 << MUX0 );

    // ADC-Prescaler = 64 (9.8304MHz / 64 = 153kHz)
    ADCSRA |= ( 1 << ADPS2 | 1 << ADPS1 );

    // Turn on ADC and execute dummy-conversion
    ADCSRA |= 1 << ADEN;
    ADCSRA |= 1 << ADSC;

    // Wait for end of dummy conversion
    while ( ADCSRA & ( 1 << ADSC ) );

    nosense  = ADCW;
    (void) nosense;
}

static uint16_t adc_read( const uint8_t channel ) {
    uint16_t result = 0;

    ADMUX &= ~( 0x07 );
    ADMUX |= channel;

    // 9 measurements, discarding the first, summing up the rest
    for ( uint8_t i = 9; i; i-- ) {
        ADCSRA |= 1 << ADSC;

        while ( ADCSRA & ( 1 << ADSC ) );

        result += ADCW;

        // Discard first measurement
        if ( i == 9 ) {
            result = 0;
        }
    }
    result >>= 3;

    return result;
}

// Calculation of battery voltage
uint8_t bat_calc( const uint8_t channel ) {

    uint8_t  result;
    uint16_t voltage_raw = adc_read( channel );

    // voltage_raw * 11 / 64 = voltage_in
    result = ( 11 * voltage_raw + 128 ) >> 8;

    return result;
}

uint8_t imp_calc( const uint8_t channel ) {
    uint16_t voltage_raw;
    uint8_t  result;

    voltage_raw = adc_read( channel );

    // With 1.1V-reference:
    // 25/512 * voltage_raw = R
    result = ( 25 * voltage_raw + 256 ) >> 9;

    // Check
    if ( !result ) {
        return 1;
    }

    return result;
}
