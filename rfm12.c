/*
 * rfm.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

#ifdef RFM12_H_

// Is TX ready for next byte or has RX received?
static inline uint8_t rfm_ready(void) {
	SDI_PORT &= ~(1 << SDI);
	uint8_t status = 0;
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	ACTIVATE_RFM;
	__asm__ __volatile__( "rjmp 1f\n 1:" );

	status = ((SDO_PIN & (1 << SDO)) && 1);

	DEACTIVATE_RFM;

	return status;
}

static inline uint8_t rfm_spi(uint8_t spibyte) {
#if(HARDWARE_SPI_12)
	SPDR = spibyte;
	while (!(SPSR & (1 << SPIF)))
		;
	spibyte = SPDR;
#else
	for (uint8_t i = 8; i; i--) {
		if (spibyte & 0x80) {
			SDI_PORT |= (1 << SDI);
		}
		else {
			SDI_PORT &= ~(1 << SDI);
		}
		spibyte <<= 1;
		SCK_PIN = (1 << SCK);
		__asm__ __volatile__( "rjmp 1f\n 1:" );
		if (SDO_PIN & (1 << SDO)) spibyte |= 1;
		else spibyte |= 0;
		SCK_PIN = (1 << SCK);
	}
#endif
	return spibyte;
}

uint16_t rfm_cmd(uint16_t command) {
	bitconverter order;
	order.zahl = command;

	SCK_PORT &= ~(1 << SCK);
	SDI_PORT &= ~(1 << SDI);
	ACTIVATE_RFM;

	order.bytes[1] = rfm_spi(order.bytes[1]);
	order.bytes[0] = rfm_spi(order.bytes[0]);

	SCK_PORT &= ~(1 << SCK);
	SDI_PORT &= ~(1 << SDI);
	DEACTIVATE_RFM;

	return order.zahl;
}

void rfm_reset(void) {
	rfm_cmd(0xFE00);
	_delay_ms(1000);
}

// Transmitter ein- und ausschalten
uint8_t rfm_txon(void) {
	rfm_cmd(0x8239); // TX on (set bit ET in Power Management)
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	return 0;
}

uint8_t rfm_txoff(void) {
	rfm_cmd(0x8209); // TX off (clear bit ET in Power Management)
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	return 0;
}

// Turn Receiver on and off
uint8_t rfm_rxon(void) {
	rfm_cmd(0x82D9); // RX on (set bit ER in Power Management)
	rfm_cmd(0xCA81); // set FIFO mode
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	rfm_cmd(0xCA83); // enable FIFO: sync word search
	return 0;
}

uint8_t rfm_rxoff(void) {
	rfm_cmd(0x8209); // RX off (clear bit ER in Power Management)
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	return 0;
}

// Byte received? (available for access in main)
uint8_t rfm_receiving(void) {
	return rfm_ready();
}

// Receive byte
static inline uint8_t rfm_rxbyte(uint8_t *errflg) {
	uint32_t utimer;
	utimer = (RFM_TIMEOUTVAL);
	uint8_t value;

	while (!rfm_ready() && utimer--)
		;
	if (!utimer) (*errflg)++;
	value = rfm_cmd(0xB000);

	return value;
}

// Transmit byte
static inline uint8_t rfm_txbyte(uint8_t value) {
	uint32_t utimer;
	utimer = (RFM_TIMEOUTVAL);
	while (!rfm_ready() && utimer--)
		;
	rfm_cmd(0xB800 + value);
	return (utimer ? 0 : 1); // 0 : successful, 1 : error
}

// Get status
uint16_t rfm_status(void) {
	uint16_t status = rfm_cmd(0x0000);
	return status;
}

// Settings concerning bitrate
static void rfm_setbit(uint32_t bitrate) {
	uint8_t bw, freqdev;

	switch (bitrate) {
		case 38400:
		case 57600:
			bw = 160;
			freqdev = 80;
			break;
		case 115200:
			bw = 128;
			freqdev = 112;
			break;
		default:
			bw = 192;
			freqdev = 32;
			break;
	}

	//Data Rate
	rfm_cmd(0xC600 | (DATARATE & 0xFF));

	// Receiver control
	rfm_cmd(0x9402 | bw);

	//Transmitter Control Command
	rfm_cmd(0x9800 | freqdev);
}

// Initialise RFM
void rfm_init(void) {
	// Configure SPI inputs and outputs
	NSEL_PORT |= (1 << NSEL);
	SDO_PORT |= (1 << SDO);
	SDO_DDR &= ~(1 << SDO);
	SDI_DDR |= (1 << SDI);
	SCK_DDR |= (1 << SCK);
	NSEL_DDR |= (1 << NSEL);

#ifdef SPCR
#if HARDWARE_SPI_12
	// Activate and configure hardware SPI at F_CPU/16
	SPCR |= (1 << SPE | 1 << MSTR | 1<<SPR0);
#endif
#endif

	_delay_ms(500);
	for (uint8_t runs = 5; runs; runs--) {

		rfm_cmd(0xC000); // CLK: 1MHz
		//Configuration Setting
		rfm_cmd(0x80C8 | ((FR_CONST_1 - (FREQUENCY < 400000000L)) << 4));		// EL,EF
		//Bitrate + further settings
		rfm_setbit(BITRATE);
		//Data Filter
		rfm_cmd(0xC2AC);		// AL,!ml,DIG,DQD4
		//FIFO/Reset Mode
		rfm_cmd(0xCA81);		// FIFO8,SYNC,!ff,DR
		// Disable low duty cycle
		rfm_cmd(0xC800);
		//Automatic Frequency Control
		rfm_cmd(0xC4F7);		// AFC settings: autotuning: -20kHz...+15kHz

		// Disable Wakeuptimer
		rfm_cmd(0xE000);

		//Power Management Setting
		rfm_cmd(0x8209);		// Oscillator on, wake-up-timer on demand

		//Frequency Setting
		rfm_cmd(0xA000 | FREQ_CHANNEL);

		_delay_ms(50);
	}
	rfm_status();
	rfm_rxon();
}

// Clear all interrupt sources
void rfm_nirq_clear(void) {
#if (USE_NIRQ)
	// Alles aus
	rfm_cmd(0x8201);
	rfm_cmd(0x8047 | ((FR_CONST_1 - (FREQUENCY < 400000000L)) << 4));

	while (!(NIRQ_PIN & (1 << NIRQ))) {
		if (rfm_status() & (1 << 15)) {
			rfm_cmd(0xB8AA);
			rfm_cmd(0xB000);
		}
	}
#endif
}

// Initialise Wake-Up-Timer
void rfm_wake_up_init(void) {
	// Wake Up Timer
	rfm_cmd(0xEB01);		// 2,048 Sekunden
	rfm_cmd(0x8201);
	rfm_cmd(0x8203);
	rfm_status();
}

// Deactivate Wake-Up-Timer
void rfm_wake_up_clear(void) {
	rfm_cmd(0x8201);
	rfm_cmd(0x80C7 | ((FR_CONST_1 - (FREQUENCY < 400000000L)) << 4));
	rfm_status();
}

// Activate Wake-Up-Timer and let uC go sleep
void rfm_set_timer_and_sleep(uint8_t mantissa, uint8_t exponent) {
	uint16_t value = (((exponent & 0x1F) << 8) | mantissa);
	rfm_cmd(0xE000 | value); // Wake-Up-Timer setzen
	rfm_cmd(0x8201);
	rfm_cmd(0x8047 | ((FR_CONST_1 - (FREQUENCY < 400000000L)) << 4));
	rfm_cmd(0x8203); // Wake-Up-Timer einschalten
	rfm_status();
}

// Transmit data
uint8_t rfm_transmit(char *data, uint8_t length) {
	uint8_t error = 0;
	uint16_t crc = CRC16_SEED;						// Set CRC-Seed

	rfm_rxoff();									// Turn off receiver
	rfm_status();									// Query status to clear potential error flags

	rfm_txon();										// Turn on transmitter

	error += rfm_txbyte(0xAA);						// Send preamble of three 10101010-Bytes
	error += rfm_txbyte(0xAA);
	error += rfm_txbyte(0xAA);

	error += rfm_txbyte(0x2D);						// Send FIFO-keyword
	error += rfm_txbyte(0xD4);

	error += rfm_txbyte(length);					// Transmit number of databytes
	crc = crc16(crc, length);						// CRC-Update

	if (length > MAX_ARRAYSIZE) length = MAX_ARRAYSIZE;

	for (uint8_t bytenum = 0; bytenum < length; bytenum++) {
		error += rfm_txbyte(data[bytenum]);			// Transmit databyte
		crc = crc16(crc, data[bytenum]);			// CRC-Update
	}

	crc ^= 0xFFFF;									// Final XOR for CRC
	error += rfm_txbyte((crc >> 8) & 0xFF);			// Transmit CRC-Highbyte
	error += rfm_txbyte(crc & 0xFF);				// Transmit CRC-Lowbyte

	error += rfm_txbyte(0xAA);						// Dummybyte
	error += rfm_txbyte(0xAA);						// Dummybyte
	error += rfm_txbyte(0xAA);						// Dummybyte

	rfm_txoff();									// TX off

	rfm_status();									// Query status to clear potential error flags

	rfm_rxon();										// Turn receiver back on

	return (error? 1:0); // 0 : successful, 1 : error
}

// Receive data
uint8_t rfm_receive(char *data, uint8_t *length) {
	uint8_t bytenum, length_local, error = 0;
	uint16_t crc_rec, crc_calc = CRC16_SEED;

	rfm_status(); 									// Query status to clear potential error flags

	length_local = rfm_rxbyte(&error); 				// Receive data length
	crc_calc = crc16(crc_calc, length_local);		// CRC-Update

	if (length_local > MAX_ARRAYSIZE) length_local = MAX_ARRAYSIZE;

	for (bytenum = 0; bytenum < length_local; bytenum++) {
		data[bytenum] = rfm_rxbyte(&error);			// Receive databyte
		crc_calc = crc16(crc_calc, data[bytenum]);	// CRC-Update
	}
	data[length_local] = '\0';

	crc_calc ^= 0xFFFF;								// Final XOR for CRC
	crc_rec = (rfm_rxbyte(&error) << 8);			// Receive CRC-Highbyte
	crc_rec |= rfm_rxbyte(&error);					// Receive CRC-Lowbyte

	rfm_cmd(0xCA81); // empty FIFO					// Reset FIFO
	rfm_status(); 									// Status abfragen, um evtl. Fehler zu löschen
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	rfm_cmd(0xCA83); 								// Restart FIFO: enable sync word search

	*length = length_local;

	return (!(crc_rec == crc_calc) || error); 		// 0 : successful, 1 : error
}

#endif
