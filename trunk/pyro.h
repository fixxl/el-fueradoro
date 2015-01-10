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

// Maximale Gr��e von Arrays
#define MAX_ARRAYSIZE 		30

// Schwellwert zum L�schen des LCD
#define DEL_THRES			251

// Value for input timeout
#define TIMEOUTVAL			(F_CPU>>4)

// Definitionen der Sende- und Empfangszust�nde
#define 	FIRE				'f'
#define 	IDENT				'i'
#define 	ERROR				'e'
#define 	PARAMETERS			'p'
#define		TEMPERATURE			't'
#define		CHANGE				'c'
#define 	IDLE				0

// Definitionen der Bytedauer
#define		BYTE_DURATION_US	(8*(1000000UL + BITRATE)/BITRATE)

// Definition der L�ngen
#define		ADDITIONAL_LENGTH	14 // Preamble 4 + Passwort 2 + Length Byte 1 + CRC 2 + Spare 5
#define		FIRE_LENGTH			4
#define		IDENT_LENGTH		4
#define		PARAMETERS_LENGTH	7
#define		TEMPERATURE_LENGTH	5
#define		CHANGE_LENGTH		6

// Wie oft sollen Z�ndkommandos gesendet werden?
#define 	FIRE_REPEATS		5
#define		IDENT_REPEATS		3
#define		CHANGE_REPEATS		3
#define		PARAMETERS_REPEATS	2
#define		TEMPERATURE_REPEATS	2

// Bitflags
typedef union {
	struct {
		unsigned uart_active :1;
		unsigned uart_config :1;
		unsigned fire :1;
		unsigned send :1;
		unsigned transmit :1;
		unsigned receive :1;
		unsigned list :1;
		unsigned lcd_update :1;
		unsigned tx_post :1;
		unsigned rx_post :1;
		unsigned show_only :1;
		unsigned reset_fired :1;
		unsigned reset_device :1;
		unsigned clear_list :1;
		unsigned hw :1;
		unsigned remote :1;
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
uint8_t debounce(volatile uint8_t *port, uint8_t pin);
uint8_t fire_command_uart_valid(const char *field);

#endif /* PYRO_H_ */
