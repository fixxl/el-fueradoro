/*
 * rfm69.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

#ifdef RFM69_H_

static inline uint8_t rfm_spi(uint8_t spibyte) {
#if HARDWARE_SPI
	SPDR = spibyte;
	while (!(SPSR & (1 << SPIF)))
		;
	spibyte = SPDR;

#else
	for (uint8_t i = 8; i; i--) {
		if (spibyte & 0x80) {
			RFM_PORT |= (1 << SDI);
		}
		else {
			RFM_PORT &= ~(1 << SDI);
		}
		spibyte <<= 1;
		RFM_PIN = (1 << SCK);
		__asm__ __volatile__( "rjmp 1f\n 1:" );
		if (RFM_PIN & (1 << SDO)) spibyte |= 1;
		else spibyte |= 0;
		RFM_PIN = (1 << SCK);
	}
#endif
	return spibyte;
}

uint8_t rfm_cmd(uint16_t command, uint8_t wnr) {
	// Split command in two bytes, merge with write-/read-flag
	uint8_t highbyte = ((command >> 8) | (wnr ? 128 : 0));
	uint8_t lowbyte = (wnr ? (command & 0x00FF) : 0xFF);

	// Ensure correct idle levels, then enable module
	RFM_PORT &= ~(1 << SCK);
	RFM_PORT &= ~(1 << SDI);
	ACTIVATE_RFM;

	// SPI-Transfer
	rfm_spi(highbyte);
	lowbyte = rfm_spi(lowbyte);

	// Disable module
	RFM_PORT &= ~(1 << SDI);
	RFM_PORT &= ~(1 << SCK);
	DEACTIVATE_RFM;

	return lowbyte;
}

uint8_t rfm_receiving(void) {
	uint8_t status;
	ACTIVATE_RFM;
	status = ((rfm_cmd(0x28FF, 0) & (1 << 2)) && 1); // PayloadReady?
	DEACTIVATE_RFM;
	return status;
}

uint16_t rfm_status(void) {
	uint16_t status = 0;
	ACTIVATE_RFM;
	status |= rfm_cmd(0x2700, 0);
	status <<= 8;
	status |= rfm_cmd(0x2800, 0);
	DEACTIVATE_RFM;
	return status;
}

//------------------------------------------------------------------------------------------------------------------------

static uint8_t rfm_fifo_wnr(char *data, uint8_t wnr) {
	uint8_t temp;

	ACTIVATE_RFM;
	// Address FIFO-Register in write- or read-mode
	rfm_spi(wnr ? 128 : 0);

	// Write data length or read data length depending on mode
	temp = rfm_spi(wnr ? data[0] : 0xFF);
	if (!wnr) data[0] = temp;

	// Make sure there's no array-overflow
	if (data[0] > MAX_ARRAYSIZE) data[0] = MAX_ARRAYSIZE;

	// Write/read data bytes
	for (uint8_t i = 1; i <= data[0]; i++) {
		temp = rfm_spi(wnr ? data[i] : 0xFF);
		if (!wnr) data[i] = temp;
	}

	DEACTIVATE_RFM;

	return 0;
}

static inline void rfm_fifo_clear(void) {
	rfm_cmd(0x2810, 1);
}

//------------------------------------------------------------------------------------------------------------------------

// Transmitter ein- und ausschalten
void rfm_txon(void) {
	uint32_t utimer;
	utimer = TIMEOUTVAL;
	rfm_cmd(0x010C, 1); // TX on (set to transmitter mode in RegOpMode)
	while (!(rfm_cmd(0x27FF, 0) & 1 << 7) && --utimer)
		; // Wait for Mode-Ready-Flag
	utimer = TIMEOUTVAL;
	while (!(rfm_cmd(0x27FF, 0) & 1 << 5) && --utimer)
		; // Wait for TX-Ready-Flag
}

void rfm_txoff(void) {
	uint32_t utimer;
	utimer = TIMEOUTVAL;
	rfm_cmd(0x0104, 1); // TX off (set to standby mode in RegOpMode)
	while (!(rfm_cmd(0x27FF, 0) & (1 << 7)) && --utimer)
		; // Wait for Mode-Ready-Flag
}

// Receiver ein- und ausschalten
void rfm_rxon(void) {
	uint32_t utimer;
	utimer = TIMEOUTVAL;
	rfm_cmd(0x0110, 1); // RX on (set to receiver mode in RegOpMode)
	while (!(rfm_cmd(0x27FF, 0) & (1 << 7)) && --utimer)
		; // Wait for Mode-Ready-Flag
	utimer = TIMEOUTVAL;
	while (!(rfm_cmd(0x27FF, 0) & (1 << 6)) && --utimer)
		; // Wait for RX-Ready-Flag
}

void rfm_rxoff(void) {
	rfm_cmd(0x0104, 1); // RX off (set to standby mode in RegOpMode)
	while (!(rfm_cmd(0x27FF, 0) & (1 << 7)))
		; // Wait for Mode-Ready-Flag
}

// RSSI-Wert auslesen
uint8_t rfm_get_rssi_dbm(void) {
	uint32_t utimer;
	utimer = TIMEOUTVAL;
	if (!rfm_cmd(0x6FFF, 0)) {
		rfm_cmd(0x2301, 1);
		while (!(rfm_cmd(0x23FF, 0) & (1 << 0)) && --utimer)
			;
	}
	return (rfm_cmd(0x24FF, 0) >> 1);
}
//------------------------------------------------------------------------------------------------------------------------

// Bitrate einstellen
static inline void rfm_setbit(uint32_t bitrate) {
	uint8_t bw;
	uint16_t freqdev;

	switch (bitrate) {
		case 38400:
		case 57600:
			bw = 1;
			freqdev = 1475;
			break;
		case 115200:
			bw = 8;
			freqdev = 1966;
			break;
		default:
			bw = 2;
			freqdev = 737;
			break;
	}
	//Frequency Deviation
	rfm_cmd(0x0500 + (freqdev >> 8), 1);
	rfm_cmd(0x0600 + (freqdev & 0xFF), 1);

	//Data Rate
	rfm_cmd(0x0300 | DATARATE_MSB, 1);
	rfm_cmd(0x0400 | DATARATE_LSB, 1);

	//Receiver Bandwidth
	rfm_cmd(0x1980 | bw, 1);

	// AFC
	// rfm_cmd(0x1A80 | bw, 1);
}

// RFM initialisieren
void rfm_init(void) {
	// Ein-/Ausgänge der SPI-Schnittstelle konfigurieren
	RFM_PORT |= (1 << NSEL);
	RFM_DDR &= ~(1 << SDO);
	RFM_DDR |= (1 << SDI) | (1 << SCK) | (1 << NSEL);

#ifdef SPCR
#if HARDWARE_SPI
	// SPI aktivieren und konfigurieren
	SPCR |= (1 << SPE | 1 << MSTR | 1 << SPR1 | 1 << SPR0);
#endif
#endif

	for (uint8_t i = 0; i < 20; i++) {
		_delay_ms(5);
		rfm_cmd(0x0200, 1); // FSK, Packet mode, No shaping

		//Bitrate + zugehörige Einstellungen (Empfängerbandbreite, Frequenzhub)
		rfm_setbit(BITRATE);

		rfm_cmd(0x131B, 1); // OCP enabled, 100mA

		// DIO-Mapping
		rfm_cmd(0x2500, 1); // Clkout, FifoFull, FifoNotEmpty, FifoLevel, PacketSent/CrcOk
		rfm_cmd(0x2607, 1); // Clock-Out off

		// Frequenzeinstellung
		rfm_cmd(0x0700 + FRF_MSB, 1);
		rfm_cmd(0x0800 + FRF_MID, 1);
		rfm_cmd(0x0900 + FRF_LSB, 1);

		// Paketkonfiguration
		rfm_cmd(0x3790, 1); // Variable length, No DC-free encoding/decoding, CRC-Check, No Address filter
		rfm_cmd(0x3800 + MAX_ARRAYSIZE, 1); // Max. Payload-Length
		rfm_cmd(0x3C80, 1); // Tx-Start-Condition festlegen
		rfm_cmd(0x3D12, 1); // Packet-Config2

		// Präambel auf 3 Bytes stellen
		rfm_cmd(0x2C00, 1);
		rfm_cmd(0x2D03, 1);

		// Sync-Mode
		rfm_cmd(0x2E88, 1); // set FIFO mode
		rfm_cmd(0x2F2D, 1); // sync word MSB
		rfm_cmd(0x30D4, 1); // sync word LSB

		// Receiver config
		rfm_cmd(0x1800, 1); // LNA: 50 Ohm Input Impedance, Automatic Gain Control
		rfm_cmd(0x582D, 1); // High sensitivity
		rfm_cmd(0x6F30, 1); // Improved DAGC
		rfm_cmd(0x29DC, 1); // RSSI mind. -110 dBm
		rfm_cmd(0x1E2D, 1); // AFC starten, Auto-On
		while (!(rfm_cmd(0x1EFF, 0) & (1 << 4)))
			;

		rfm_cmd(0x1180 + P_OUT, 1); // Set Output Power
	}
	rfm_cmd(0x0A80, 1); // Start RC-Oscillator
	while (!(rfm_cmd(0x0A00, 0) & (1 << 6)))
		;
	// Wait for RC-Oscillator

	rfm_rxon();
}

// Datenstrom senden
uint8_t rfm_transmit(char *data, uint8_t length) {
	uint32_t utimer;
	utimer = TIMEOUTVAL;
	char fifoarray[MAX_ARRAYSIZE + 1];

	rfm_rxoff(); // Empfänger ausschalten
	rfm_fifo_clear();

	fifoarray[0] = length; // Anzahl zu übertragender Bits senden

	for (uint8_t i = 0; i < length; i++) {
		fifoarray[1 + i] = data[i];
	}
	fifoarray[length + 1] = '\0';

	while (!(rfm_cmd(0x27FF, 0) & (1 << 7)) && --utimer)
		;

	rfm_fifo_wnr(fifoarray, 1); // Daten in FIFO schreiben

	rfm_txon(); // Sender einschalten

	while (!(rfm_cmd(0x28FF, 0) & (1 << 3)))
		; // Auf PackageSent warten
	rfm_rxon();

	return 0;
}

// Datenstrom empfangen
uint8_t rfm_receive(char *data, uint8_t *length) {

	rfm_rxoff();

	char fifoarray[MAX_ARRAYSIZE + 1];
	uint8_t length_local;

	rfm_fifo_wnr(fifoarray, 0); // Daten aus FIFO lesen

	length_local = fifoarray[0];

	if (length_local > MAX_ARRAYSIZE - 1) length_local = MAX_ARRAYSIZE - 1;

	for (uint8_t i = 0; i < length_local; i++) {
		data[i] = fifoarray[i + 1];
	}
	data[length_local] = '\0';
	rfm_fifo_clear();

	rfm_rxon();

	*length = length_local;

	return 1;
}

#endif
