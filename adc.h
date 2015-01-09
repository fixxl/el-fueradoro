/*
 * adc.h
 * Einstellungen und Funktionen für den A/D-Umsetzer
 */

#ifndef ADC_H_
#define ADC_H_

void adc_init(void);
void adc_deinit(void);
uint8_t adc_read(uint8_t channel);



#endif
