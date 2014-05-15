/*
 * pyro.h
 *
 *  Created on: 10.07.2013
 *      Author: Felix
 */

#ifndef PYRO_H_
#define PYRO_H_

// Anschluss des Schl�sselschalters
#define KEY_PORT			C
#define KEY_NUM				4

#define DEL_THRES			250
#define MAX_ARRAYSIZE 		30

#define SENDERBOX 			((slave_id==0) && (unique_id==0))
#define KEYDDR				DDR(KEY_PORT)
#define KEYPIN				PIN(KEY_PORT)
#define KEYPORT 			PORT(KEY_PORT)
#define KEY					KEY_NUM
#if(KEY_PORT==C)
#define KEYINT 				PCINT1_vect
#elif(KEY_PORT==B)
#define KEYINT				PCINT0_vect
#else
#define KEYINT 				PCINT2_vect
#endif


#define DS18B20				'o'
#define DHT22				'd'

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init1")));

void create_symbols(void);
void key_init(void);
void key_deinit(void);
uint8_t debounce (volatile uint8_t *port, uint8_t pin);


#endif /* PYRO_H_ */
