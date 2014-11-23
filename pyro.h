/*
 * pyro.h
 *
 *  Created on: 10.07.2013
 *      Author: Felix
 */

#ifndef PYRO_H_
#define PYRO_H_

// Anschluss des Schlüsselschalters
#define KEY_PORT			C
#define KEY_NUM				4

// Wie oft sollen Zündkommandos gesendet werden?
#define FIREREPEATS			5

// Maximale Größe von Arrays
#define MAX_ARRAYSIZE 		30

// Schwellwert zum Löschen des LCD
#define DEL_THRES			251

// Value for input timeout
#define TIMEOUTVAL			(F_CPU>>4)

// Definitionen der Sende- und Empfangszustände
#define 	FIRE			'f'
#define 	IDENT			'i'
#define 	REPEAT			'r'
#define 	ERROR			'e'
#define 	PARAMETERS		'p'
#define 	ACKNOWLEDGED	'a'
#define 	SETUP			's'
#define		TEMPERATURE		't'
#define 	IDLE			0


// Bitflags
typedef union{
	struct{
	unsigned uart_active :1;
	unsigned uart_config :1;
	unsigned fire :1;
	unsigned send :1;
	unsigned transmit :1;
	unsigned receive :1;
	unsigned list :1;
	unsigned lcd_update:1;
	unsigned tx_post:1;
	unsigned rx_post:1;
	unsigned show_only:1;
	unsigned reset_fired:1;
	unsigned reset_device:1;
	unsigned clear_list:1;
	unsigned temp:1;
	unsigned hw: 1;
	} b;
	uint16_t complete;
} bitfeld_t;


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


// Speicher-Einstellungen zu IDs im EEPROM
#define ANFANG_ID_SPEICHER	24
#define STEP_ID_SPEICHER	36
#define CRC_ID_SPEICHER		16
#define ID_MESS				!(eeread(ANFANG_ID_SPEICHER)==eeread(ANFANG_ID_SPEICHER+STEP_ID_SPEICHER)) && (eeread(ANFANG_ID_SPEICHER)==eeread(ANFANG_ID_SPEICHER+2*STEP_ID_SPEICHER)) && (eeread(ANFANG_ID_SPEICHER+1)==eeread(ANFANG_ID_SPEICHER+1+STEP_ID_SPEICHER)) && (eeread(ANFANG_ID_SPEICHER+1)==eeread(ANFANG_ID_SPEICHER+1+2*STEP_ID_SPEICHER))


// Temperatursensoren
#define DS18B20				'o'
#define DHT22				'd'


// Funktionsprototypen
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init1")));
void create_symbols(void);
void key_init(void);
void key_deinit(void);
uint8_t debounce (volatile uint8_t *port, uint8_t pin);
uint8_t uart_valid(const char *field);


#endif /* PYRO_H_ */
