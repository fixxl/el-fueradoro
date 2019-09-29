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

void timer1_init( void );
void timer1_on( void );
void timer1_off( void );
void timer1_reset( void );
#endif