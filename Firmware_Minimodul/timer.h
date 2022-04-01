/*
 * timer.h
 *
 * Einstellungen zum Timer
 * Wird genutzt f�r UART- und RFM12-Timeouts, um Programmh�nger zu vermeiden
 */

#ifndef TIMER_H_
#define TIMER_H_

#define TIMER_MEASURE_FLAG         1
#define TIMER_TRANSMITCOUNTER_FLAG 2
#define TIMER_DEBOUNCE_FLAG        4
#define TIMER_IMPEDANCE_CTR_FLAG   8

// Basic interrupt interval (10 ms)
#define T1_INTERRUPT_INTERVAL_MS   10ULL

void timer1_init( void );
void timer1_on( void );
void timer1_off( void );
void timer1_reset( void );
#endif
