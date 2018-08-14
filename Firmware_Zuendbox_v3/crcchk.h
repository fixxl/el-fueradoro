/*
 * crcchk.h
 * Funktionen zur CRC-Berechnung
 */

#ifndef CRCCHK_H_
#define CRCCHK_H_

#define CRC16_SEED    0x1D0F
#define CRC8_SEED     0

uint16_t crc16(uint16_t crc, uint8_t data);
uint8_t crc8(uint8_t crc, uint8_t data);
uint16_t crcwert(char *feld, uint8_t startindex, uint8_t laenge, uint16_t crc, uint8_t crctype);
#endif
