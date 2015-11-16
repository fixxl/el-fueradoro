/*
 * crcchk.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// CRC16 according to Semtech-CCITT (Final result has to be XORed bitwise before transmission/comparison!)
uint16_t crc16(uint16_t crc, uint8_t data) {
	crc ^= (data << 8);

	for (uint8_t i = 8; i; i--) {
		data = ((crc & 0x8000) && 1); // Wird durch && entweder 0 oder 1
		crc <<= 1;

		if (data) { crc ^= 0x1021; } // 0x1021 due to polynom x^16+x^12+x^5+1 and MSB first
	}

	return crc;
}

// CRC8 according to MAXIM for byte "data" with register preload "crc"
uint8_t crc8(uint8_t crc, uint8_t data) {
	crc ^= data;

	for (uint8_t i = 8; i; i--) {
		data = (crc & 0x01);
		crc >>= 1;

		if (data) { crc ^= 0x8C; } // 0x8C due to polynom x^8+x^5+x^4+1 and LSB first
	}

	return crc;
}

// Automatic CRC-calculation for bitstream in array
// "crctype" musst be 8 or 16, final XOR for CRC16 is executed automatically
uint16_t crcwert(char *feld, uint8_t startindex, uint8_t laenge, uint16_t crc, uint8_t crctype) {
	if (crctype == 8) {
		for (uint8_t zaehl = 0; zaehl < laenge; zaehl++) {
			crc = crc8(crc, feld[startindex + zaehl]);
		}
	}

	if (crctype == 16) {
		for (uint8_t zaehl = 0; zaehl < laenge; zaehl++) {
			crc = crc16(crc, feld[startindex + zaehl]);
		}

		crc ^= 0xFFFF;
	}

	return crc;
}
