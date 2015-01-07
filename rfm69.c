/*
 * rfm69.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

#ifdef RFM69_H_

static inline uint8_t rfm_spi(uint8_t spibyte) {
#if(HARDWARE_SPI_69)
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
		if (SDO_PIN & (1 << SDO)) spibyte |= 0x01;
		else spibyte &= 0xFE;
		SCK_PIN = (1 << SCK);
	}
#endif
	return spibyte;
}

// Send 16-bit-command to RFM69 (8 bits register-number, 8 bits command)
uint8_t rfm_cmd(uint16_t command, uint8_t wnr) {
	// Split command in two bytes, merge with write-/read-flag
	uint8_t highbyte = ((command >> 8) | (wnr ? 128 : 0));
	uint8_t lowbyte = (wnr ? (command & 0x00FF) : 0xFF);

	// Ensure correct idle levels, then enable module
	SCK_PORT &= ~(1 << SCK);
	SDI_PORT &= ~(1 << SDI);
	ACTIVATE_RFM;

	// SPI-Transfer
	rfm_spi(highbyte);
	lowbyte = rfm_spi(lowbyte);

	// Disable module
	SDI_PORT &= ~(1 << SDI);
	SCK_PORT &= ~(1 << SCK);
	DEACTIVATE_RFM;

	return lowbyte;
}

uint8_t rfm_receiving(void) {
	uint8_t status;
	ACTIVATE_RFM;
	status = rfm_cmd(0x28FF, 0);
	DEACTIVATE_RFM;
	// Check if PayloadReady is set AND unused bit is not set (if bit 0 is set, module is not plugged in)
	return ((status & (1 << 2)) && !(status & (1 << 0)));
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

static uint8_t rfm_fifo_wnr(char *data, uint8_t wnr) { // Address FIFO-Register in write- or read-mode

	ACTIVATE_RFM;

	// Write data bytes
	if (wnr) {
		// Make sure there's no array-overflow
		if (data[0] > MAX_ARRAYSIZE) data[0] = MAX_ARRAYSIZE;

		// Write-access register 0
		rfm_spi(0x80);

		// Transfer length
		rfm_spi(data[0]);

		// Transfer data
		for (uint8_t i = 1; i <= data[0]; i++) {
			rfm_spi(data[i]);
		}
	}

	// Read data bytes
	else {
		// Read-access register 0
		rfm_spi(0);

		// Get length
		data[0] = rfm_spi(0xFF);

		// Make sure there's no array-overflow
		if (data[0] > MAX_ARRAYSIZE) data[0] = MAX_ARRAYSIZE;

		// Get data
		for (uint8_t i = 1; i <= data[0]; i++) {
			data[i] = rfm_spi(0xFF);
		}
	}

	DEACTIVATE_RFM;

	return 0;
}

static inline void rfm_fifo_clear(void) {
	rfm_cmd(0x2810, 1);
}

//------------------------------------------------------------------------------------------------------------------------

// Turn Transmitter on and off
uint8_t rfm_txon(void) {
	uint32_t utimer = RFM69_TIMEOUTVAL;
	rfm_cmd(0x010C, 1); // TX on (set to transmitter mode in RegOpMode)
	while (--utimer && !(rfm_cmd(0x27FF, 0) & (1 << 7)))
		; // Wait for Mode-Ready- and TX-Ready-Flag
	return (utimer ? 0 : 1);
}

uint8_t rfm_txoff(void) {
	uint32_t utimer = RFM69_TIMEOUTVAL;
	rfm_cmd(0x0104, 1); // TX off (set to standby mode in RegOpMode)
	while (--utimer && !(rfm_cmd(0x27FF, 0) & (1 << 7)))
		; // Wait for Mode-Ready-Flag
	return (utimer ? 0 : 1);
}

// Turn Receiver on and off
uint8_t rfm_rxon(void) {
	uint32_t utimer = RFM69_TIMEOUTVAL;
	rfm_cmd(0x3D04 | rfm_cmd(0x3DFF, 0), 1);
	rfm_cmd(0x0110, 1); // RX on (set to receiver mode in RegOpMode)
	while (--utimer && !(rfm_cmd(0x27FF, 0) & (1 << 7)))
		; // Wait for Mode-Ready--Flag
	return (utimer ? 0 : 1);
}

uint8_t rfm_rxoff(void) {
	uint32_t utimer = RFM69_TIMEOUTVAL;
	rfm_cmd(0x0104, 1); // RX off (set to standby mode in RegOpMode)
	while (--utimer && !(rfm_cmd(0x27FF, 0) & (1 << 7)))
		; // Wait for Mode-Ready-Flag
	return (utimer ? 0 : 1);
}

// Get RSSI-Value
uint8_t rfm_get_rssi_dbm(void) {
	uint32_t utimer = RFM69_TIMEOUTVAL;
	if (!rfm_cmd(0x6FFF, 0)) {
		rfm_cmd(0x2301, 1);
		while (!(rfm_cmd(0x23FF, 0) & (1 << 1)) && --utimer)
			;
	}
	return (rfm_cmd(0x24FF, 0) >> 1);
}

//------------------------------------------------------------------------------------------------------------------------

// Bitrate config according to RFM12-recommendations
static inline void rfm_setbit(uint32_t bitrate) {
	uint8_t bw;
	uint16_t freqdev;

	switch (bitrate) {
		case 38400:
		case 57600:
			bw = 0x02; // 125 kHz
			freqdev = 1475;
			break;
		case 115200:
			bw = 0x09; // 200 kHz
			freqdev = 1966;
			break;
		default:
			bw = 0x03; // 62.5 kHz
			freqdev = 737;
			break;
	}
	//Frequency Deviation
	rfm_cmd(0x0500 | (freqdev >> 8), 1);
	rfm_cmd(0x0600 | (freqdev & 0xFF), 1);

	//Data Rate
	rfm_cmd(0x0300 | DATARATE_MSB, 1);
	rfm_cmd(0x0400 | DATARATE_LSB, 1);

	//Receiver Bandwidth
	rfm_cmd(0x1940 | bw, 1);

	// AFC
	// rfm_cmd(0x1A60 | bw, 1);
}

// Initialise RFM
void rfm_init(void) {
	uint32_t utimer;

	// Configure SPI inputs and outputs
	NSEL_PORT |= (1 << NSEL);
	SDO_PORT |= (1 << SDO);
	SDO_DDR &= ~(1 << SDO);
	SDI_DDR |= (1 << SDI);
	SCK_DDR |= (1 << SCK);
	NSEL_DDR |= (1 << NSEL);

#ifdef SPCR
#if HARDWARE_SPI_69
	// Activate and configure hardware SPI at F_CPU/16
	SPCR |= (1 << SPE | 1 << MSTR | 1 << SPR0);
#endif
#endif

	for (uint8_t i = 0; i < 20; i++) {
		_delay_ms(5);
		rfm_cmd(0x0200, 1); // FSK, Packet mode, No shaping

		//Bitrate + corresponding settings (Receiver bandwidth, frequency deviation)
		rfm_setbit(BITRATE);

		rfm_cmd(0x131B, 1); // OCP enabled, 100mA

		// DIO-Mapping
		rfm_cmd(0x2500, 1); // Clkout, FifoFull, FifoNotEmpty, FifoLevel, PacketSent/CrcOk
		rfm_cmd(0x2607, 1); // Clock-Out off

		// Carrier frequency
		rfm_cmd(0x0700 | FRF_MSB, 1);
		rfm_cmd(0x0800 | FRF_MID, 1);
		rfm_cmd(0x0900 | FRF_LSB, 1);

		// Packet config
		rfm_cmd(0x3790, 1); // Variable length, No DC-free encoding/decoding, CRC-Check, No Address filter
		rfm_cmd(0x3800 + MAX_ARRAYSIZE, 1); // Max. Payload-Length
		rfm_cmd(0x3C80, 1); // Tx-Start-Condition: FIFO not empty
		rfm_cmd(0x3D40, 1); // Packet-Config2

		// Preamble length 4 bytes
		rfm_cmd(0x2C00, 1);
		rfm_cmd(0x2D04, 1);

		// Sync-Mode
		rfm_cmd(0x2E88, 1); // set FIFO mode
		rfm_cmd(0x2F2D, 1); // sync word MSB to 0x2D
		rfm_cmd(0x30D4, 1); // sync word LSB to 0xD4

		// Receiver config
		rfm_cmd(0x1800, 1); // LNA: 50 Ohm Input Impedance, Automatic Gain Control
		rfm_cmd(0x582D, 1); // High sensitivity mode
		rfm_cmd(0x6F30, 1); // Improved DAGC
		rfm_cmd(0x29DC, 1); // RSSI mind. -110 dBm
		rfm_cmd(0x1E00, 1); // No AFC or FEI

		rfm_cmd(0x1180 + P_OUT, 1); // Set Output Power
	}
	rfm_cmd(0x0A80, 1); // Start RC-Oscillator

	utimer = RFM69_TIMEOUTVAL;
	while (!(rfm_cmd(0x0A00, 0) & (1 << 6)) && --utimer)
		; // Wait for RC-Oscillator

	rfm_rxon();
}

// Transmit data stream
uint8_t rfm_transmit(char *data, uint8_t length) {
	uint32_t utimer = RFM69_TIMEOUTVAL;
	char fifoarray[MAX_ARRAYSIZE + 1];

	// Turn off receiver, switch to Standby
	rfm_rxoff();

	// Clear FIFO
	rfm_fifo_clear();

	// Limit length
	if (length > MAX_ARRAYSIZE - 1) length = MAX_ARRAYSIZE - 1;

	// Write data to FIFO-array
	fifoarray[0] = length; // Number of data bytes
	for (uint8_t i = 0; i < length; i++) { // Data bytes
		fifoarray[1 + i] = data[i];
	}
	fifoarray[length + 1] = '\0'; // Terminate string

	// Write data to FIFO
	rfm_fifo_wnr(fifoarray, 1);

	// Turn on transmitter (Transmitting starts automatically if FIFO not empty)
	rfm_txon();

	// Wait for Package Sent
	utimer = RFM69_TIMEOUTVAL;
	while (!(rfm_cmd(0x28FF, 0) & (1 << 3)) && --utimer)
		;

	rfm_rxon();

	return (utimer ? 0 : 1); // 0 : successful, 1 : error
}

// Receive data stream
uint8_t rfm_receive(char *data, uint8_t *length) {
	char fifoarray[MAX_ARRAYSIZE + 1];
	uint8_t length_local;

	// Turn off receiver, switch to Standby
	rfm_rxoff();

	// Read FIFO-data into FIFO-array
	rfm_fifo_wnr(fifoarray, 0);

	// Read data from FIFO-array
	length_local = fifoarray[0]; // Number of data bytes
	if (length_local > MAX_ARRAYSIZE - 1) length_local = MAX_ARRAYSIZE - 1; // Limit length
	for (uint8_t i = 0; i < length_local; i++) {
		data[i] = fifoarray[i + 1]; // Data bytes
	}
	data[length_local] = '\0'; // Terminate string

	// Clear FIFO after readout (not necessary, FIFO is cleared anyway when switching from STANDBY to RX)
	rfm_fifo_clear();

	// Turn receiver back on
	rfm_rxon();

	// Write local variable to pointer
	*length = length_local;

	// Return value is for compatibility reasons with RFM12
	// It's always 0 because PayloadReady only occurs after successful hardware CRC
	return 0; // 0 : successful, 1 : error
}

#endif
