/*
 * adc.h
 * Einstellungen und Funktionen f�r den A/D-Umsetzer
 */

#ifndef ADC_H_
#define ADC_H_

void adc_init( void );
void adc_deinit( void );
uint8_t bat_calc( const uint8_t channel );
uint8_t imp_calc( const uint8_t channel );
#endif