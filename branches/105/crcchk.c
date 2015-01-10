/*
 * crcchk.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// CRC16 gemäß Semtech-CCITT (Finales Ergebnis muss vor Senden/Prüfen bitweise invertiert werden!)
uint16_t crc16(uint16_t crc, uint8_t data) {
	crc ^= (data << 8);
	for (uint8_t i = 0; i < 8; i++) {
		data = ((crc & 0x8000) && 1); // Wird durch && entweder 0 oder 1
		crc <<= 1;
		if (data) crc ^= 0x1021; // 0x1021 weil Generatorpolynom x^16+x^12+x^5+1 und MSB first
	}
	return crc;
}

// CRC8-Berechnung a la MAXIM für ein Byte bei vorgegebenem Startwert
uint8_t crc8(uint8_t crc, uint8_t data) {
	crc ^= data;
	for (uint8_t i = 0; i < 8; i++) {
		data = (crc & 0x01);
		crc >>= 1;
		if (data) crc ^= 0x8C; // 0x8C weil Generatorpolynom x^8+x^5+x^4+1 und LSB first
	}
	return crc;
}

// Automatisierte CRC-Berechnung für in Arrayform vorliegende Bitfolge.
// Übergabewert "crctype" muss 8 oder 16 sein, abschließende Invertierung bei CRC16 wird durchgeführt
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
