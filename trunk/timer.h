/*
 * timer.h
 *
 * Einstellungen zum Timer
 * Wird genutzt für UART- und RFM12-Timeouts, um Programmhänger zu vermeiden
 */

#ifndef TIMER_H_
#define TIMER_H_

#define TIMER_OVERFLOW (TIFR1 & (1<<TOV1))

void timer1_on(void);
void timer1_off(void);
extern void timer1_reset(void);



#endif
