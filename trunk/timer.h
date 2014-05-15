/*
 * timer.h
 *
 * Einstellungen zum Timer
 * Wird genutzt f�r UART- und RFM12-Timeouts, um Programmh�nger zu vermeiden
 */

#ifndef TIMER_H_
#define TIMER_H_

#define TIMER_OVERFLOW (TIFR1 & (1<<TOV1))

void timer_on(void);
void timer_off(void);
extern void timer_reset(void);



#endif
