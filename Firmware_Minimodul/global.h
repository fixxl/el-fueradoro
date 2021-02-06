/*
 * global.h
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifndef MCU
    #define MCU "atmega328p"
#endif

#ifndef RFM
    #define RFM 69
#endif

#define STRINGIZE( x )          # x
#define STRINGIZE_VALUE_OF( x ) STRINGIZE( x )

#ifndef FIRE_CHANNELS
    #define FIRE_CHANNELS 4
#endif

#define SR_CHANNELS ((FIRE_CHANNELS + 7) / 8) * 8   // Round up FIRE_CHANNELS to the next multiple of 8

// Includes und Defines
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include "portmakros.h"
#include "timer.h"
#include "eeprom.h"
#include "crcchk.h"
#include "pyro.h"
#include "leds.h"
#include "addresses.h"
#include "uart.h"
#include "terminal.h"
#include "adc.h"
#include "shiftregister.h"
#include "1wire.h"

#if RFM == 69
 #include "rfm69.h"
#elif RFM == 12
 #include "rfm12.h"
#else
 #error "No valid RF-transceiver selected!"
#endif

// Global Variables
// extern volatile uint8_t transmit_flag, key_flag;
// extern volatile uint16_t clear_lcd_tx_flag, clear_lcd_rx_flag, hist_del_flag;
#endif /* GLOBAL_H_ */
