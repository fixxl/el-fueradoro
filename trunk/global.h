/*
 * global.h
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifndef RFM
#define RFM 12
#endif

// Includes und Definitionen
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include "pyro.h"
#include "crcchk.h"
#include "eeprom.h"
#include "portmakros.h"
#include "functionprotos.h"
#include "leds.h"
#include "timer.h"
#include "addresses.h"
#include "uart.h"
#include "terminal.h"
#include "adc.h"
#include "lcd.h"
#include "shiftregister.h"
#include "dht.h"
#include "1wire.h"

#if RFM==69
#include "rfm69.h"
#elif RFM==12
#include "rfm12.h"
#else
#error "No valid RF-transceiver selected!"
#endif

// Globale Variablen
extern volatile uint8_t transmit_flag, key_flag;
extern volatile uint16_t clear_lcd_tx_flag, clear_lcd_rx_flag, hist_del_flag;

#define DEBUG 				0

#define FIREREPEATS			5

#define ANFANG_ID_SPEICHER	24
#define STEP_ID_SPEICHER	36
#define CRC_ID_SPEICHER		16
#define ID_MESS				!(eeread(ANFANG_ID_SPEICHER)==eeread(ANFANG_ID_SPEICHER+STEP_ID_SPEICHER)) && (eeread(ANFANG_ID_SPEICHER)==eeread(ANFANG_ID_SPEICHER+2*STEP_ID_SPEICHER)) && (eeread(ANFANG_ID_SPEICHER+1)==eeread(ANFANG_ID_SPEICHER+1+STEP_ID_SPEICHER)) && (eeread(ANFANG_ID_SPEICHER+1)==eeread(ANFANG_ID_SPEICHER+1+2*STEP_ID_SPEICHER))

#if ((W1PORT>TEMPHUM_PORT)+(W1PORT<TEMPHUM_PORT)+(W1NUM>TEMPHUM_NUM)+(W1NUM<TEMPHUM_NUM))
#error Ungleiche Portzuweisungen bei Temperatursensoren!
#endif

#endif /* GLOBAL_H_ */
