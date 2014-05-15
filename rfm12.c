/*
 * rfm.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

#ifdef RFM12_H_

static inline uint8_t rfm_ready(void) {
	uint8_t status = 1;
	ACTIVATE_RFM
	;
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	__asm__ __volatile__( "rjmp 1f\n 1:" );

	status = ((RFM_PIN & (1 << SDO)) && 1);

	DEACTIVATE_RFM
	;

	return status;
}

static inline uint8_t rfm_spi(uint8_t spibyte) {
#if(USE_HARDWARE_SPI)
	SPDR = spibyte;
	while (!(SPSR & (1 << SPIF)))
	;
	spibyte = SPDR;
#else
	for (uint8_t i = 8; i; i--) {
		if (spibyte & 0x80)
		RFM_PORT |= (1 << SDI);
		else
		RFM_PORT &= ~(1 << SDI);
		spibyte <<= 1;
		RFM_PIN |= (1 << SCK);
		__asm__ __volatile__( "rjmp 1f\n 1:" );
		if (RFM_PIN & (1 << SDO)) spibyte |= 1;
		RFM_PIN |= (1 << SCK);
	}
#endif
	return spibyte;
}

uint16_t rfm_cmd(uint16_t command) {
	bitconverter order;
	order.zahl = command;

	ACTIVATE_RFM
	;

	order.bytes[1] = rfm_spi(order.bytes[1]);
	order.bytes[0] = rfm_spi(order.bytes[0]);

	DEACTIVATE_RFM
	;

	return order.zahl;
}

void rfm_reset(void) {
	rfm_cmd(0xFE00);
	_delay_ms(1000);
}

// Transmitter ein- und ausschalten
void rfm_txon(void) {
	rfm_cmd(0x8239); // TX on (set bit ET in Power Management)
	__asm__ __volatile__( "rjmp 1f\n 1:" );
}

void rfm_txoff(void) {
	rfm_cmd(0x8209); // TX off (clear bit ET in Power Management)
	__asm__ __volatile__( "rjmp 1f\n 1:" );
}

// Receiver ein- und ausschalten
void rfm_rxon(void) {
	rfm_cmd(0x82D9); // RX on (set bit ER in Power Management)
	rfm_cmd(0xCA81);// set FIFO mode
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	rfm_cmd(0xCA83);// enable FIFO: sync word search
}

void rfm_rxoff(void) {
	rfm_cmd(0x8209); // RX off (clear bit ER in Power Management)
	__asm__ __volatile__( "rjmp 1f\n 1:" );
}

// Byte empfangen
static inline uint8_t rfm_rxbyte(void) {
	uint32_t utimer;
	utimer = (F_CPU << 2);
	uint8_t value;

	while (!rfm_ready() && utimer--)
	;
	value = rfm_cmd(0xB000);

	return value;
}

// Byte senden
static inline void rfm_txbyte(uint8_t value) {
	uint32_t utimer;
	utimer = (F_CPU << 2);
	while (!rfm_ready() && utimer--)
	;
	rfm_cmd(0xB800 + value);
}

uint16_t rfm_status(void) {
	uint16_t status = rfm_cmd(0x0000);
	return status;
}

uint8_t rfm_receiving(void) {
	uint8_t status;
	ACTIVATE_RFM
	;
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	__asm__ __volatile__( "rjmp 1f\n 1:" );
	status = ((RFM_PIN & (1 << SDO)) && 1);
	DEACTIVATE_RFM
	;
	return status;
}

// Bitrate einstellen
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

// RFM initialisieren
void rfm_init(void) {
	// Ein-/Ausgänge der SPI-Schnittstelle konfigurieren
#if(USE_NIRQ)
	RFM_DDR &= ~((1 << NIRQ));
	RFM_PORT |= ((1 << NIRQ));
#endif
	RFM_PORT |= ((1 << NSEL) | (1 << SDO));
	RFM_DDR &= ~((1 << SDO));
	RFM_DDR |= ((1 << SDI) | (1 << SCK) | (1 << NSEL));
	RFM_PORT &= ~((1 << SDI) | (1 << SCK));

#ifdef SPCR
#if USE_HARDWARE_SPI
	// SPI aktivieren und konfigurieren
	SPCR |= (1 << SPE | 1 << MSTR | 1 << SPR1 | 1 << SPR0);
#endif
#endif

	_delay_ms(500);
	for (uint8_t runs = 5; runs; runs--) {

		rfm_cmd(0xC000); // CLK: 1MHz
		//Configuration Setting
		rfm_cmd(0x80C8 | ((FR_CONST_1 - (FREQUENCY < 400000000L)) << 4));// EL,EF
		//Bitrate + weitere Einstellungen
		rfm_setbit(BITRATE);
		//Data Filter
		rfm_cmd(0xC2AC);// AL,!ml,DIG,DQD4
		//FIFO/Reset Mode
		rfm_cmd(0xCA81);// FIFO8,SYNC,!ff,DR
		// Disable low duty cycle
		rfm_cmd(0xC800);
		//Automatic Frequency Control
		rfm_cmd(0xC4F7);// AFC settings: autotuning: -40kHz...+35kHz

		// Disable Wakeuptimer
		rfm_cmd(0xE000);

		//Power Management Setting
		rfm_cmd(0x8209);// Oscillator on, wake-up-timer on demand

		//Frequency Setting
		rfm_cmd(0xA000 | FREQ_CHANNEL);
	}
	rfm_status();
	rfm_rxon();
}

void rfm_nirq_clear(void) {
#if (USE_NIRQ)
	// Alles aus
	rfm_cmd(0x8201);
	rfm_cmd(0x8047 | ((FR_CONST_1 - (FREQUENCY < 400000000L)) << 4));

	while (!(RFM_PIN & (1 << NIRQ))) {
		if (rfm_status() & (1 << 15)) {
			rfm_cmd(0xB8AA);
			rfm_cmd(0xB000);
		}
	}
#endif
}

void rfm_wake_up_init(void) {
	// Wake Up Timer
	rfm_cmd(0xEB01);// 2,048 Sekunden
	rfm_cmd(0x8201);
	rfm_cmd(0x8203);
	rfm_status();
}

void rfm_wake_up_clear(void) {
	rfm_cmd(0x8201);
	rfm_cmd(0x80C7 | ((FR_CONST_1 - (FREQUENCY < 400000000L)) << 4));
	rfm_status();
}

void rfm_set_timer_and_sleep(uint8_t mantissa, uint8_t exponent) {
	uint16_t value = (((exponent & 0x1F) << 8) | mantissa);
	rfm_cmd(0xE000 | value); // Wake-Up-Timer setzen
	rfm_cmd(0x8201);
	rfm_cmd(0x8047 | ((FR_CONST_1 - (FREQUENCY < 400000000L)) << 4));
	rfm_cmd(0x8203);// Wake-Up-Timer einschalten
	rfm_status();
}

// Datenstrom senden
uint8_t rfm_transmit(char *data, uint8_t length) {
	uint16_t crc = 0x1D0F;

	rfm_rxoff(); // Empfänger ausschalten
	rfm_status();// Status abfragen, um evtl. Fehler zu löschen

	rfm_txon();// Sender einschalten

	rfm_txbyte(0xAA);// 10101010-Serien ins 16-Bit-Senderegister schreiben
	rfm_txbyte(0xAA);
	rfm_txbyte(0xAA);

	rfm_txbyte(0x2D);// Schlüsselwort zur FIFO-Freigabe senden
	rfm_txbyte(0xD4);

	rfm_txbyte(length);// Anzahl zu übertragender Datenbytes senden
	crc = crc16(crc, length);

	if (length > MAX_ARRAYSIZE) length = MAX_ARRAYSIZE;

	for (uint8_t bytenum = 0; bytenum < length; bytenum++) {
		rfm_txbyte(data[bytenum]);
		crc = crc16(crc, data[bytenum]);
	}
	crc ^= 0xFFFF;
	rfm_txbyte((crc >> 8) & 0xFF);
	rfm_txbyte(crc & 0xFF);

	rfm_txbyte(0xAA); // Dummybyte, um FIFO voll zu halten
	rfm_txbyte(0xAA);// Dummybyte, um FIFO voll zu halten
	rfm_txbyte(0xAA);// Dummybyte, um FIFO voll zu halten

	rfm_txoff();// TX off

	rfm_status();
	rfm_rxon();

	return 0;
}

// Datenstrom empfangen
uint8_t rfm_receive(char *data, uint8_t *length) {
	uint8_t bytenum, length_local;
	uint16_t crc_rec, crc_calc = 0x1D0F;

	rfm_status(); // Status abfragen, um evtl. Fehler zu löschen

	length_local = rfm_rxbyte();// Datenlänge empfangen (ohne Längenbyte und CRC-Bytes)
	crc_calc = crc16(crc_calc, length_local);

	if (length_local > MAX_ARRAYSIZE) length_local = MAX_ARRAYSIZE;

	for (bytenum = 0; bytenum < length_local; bytenum++) {
		data[bytenum] = rfm_rxbyte();
		crc_calc = crc16(crc_calc, data[bytenum]);
	}
	data[length_local] = '\0';
	crc_calc ^= 0xFFFF;

	crc_rec = (rfm_rxbyte() << 8);
	crc_rec |= rfm_rxbyte();

	rfm_cmd(0xCA81); // empty FIFO
	for (uint8_t i = 0; i < 5; i++) {
		__asm__ __volatile__( "rjmp 1f\n 1:" );
	}
	rfm_cmd(0xCA83); // enable FIFO: sync word search
	*length = length_local;
	return (crc_rec == crc_calc);
}

#endif
