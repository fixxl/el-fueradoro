/*
 * eeprom.h
 *
 * Funktionen für Zugriff auf eeprom gemäß ATMEGA168PA-Datenblatt
 */

#ifndef EEPROM_H_
	#define EEPROM_H_

	uint8_t eeread(uint16_t address);
	void eewrite(uint8_t data, uint16_t address);



#endif
