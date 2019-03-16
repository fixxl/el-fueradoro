/*
 * timer.h
 *
 * Einstellungen zum Timer
 * Wird genutzt f�r UART- und RFM12-Timeouts, um Programmh�nger zu vermeiden
 */

#ifndef TIMER_H_
#define TIMER_H_

#define TIMER_OVERFLOW    (TIFR1 & (1 << TOV1))

void timer1_init(void);
void timer1_on(void);
void timer1_off(void);
extern void timer1_reset(void);
#endif
