/*
 * addresses.c
 *
 *  Created on: 06.11.2013
 *      Author: Felix
 */

#include "global.h"

// Pr�froutine, ob Sender- oder Z�ndbox
void update_addresses(uint8_t *uniqueid, uint8_t *slaveid) {
	uint8_t sid_local = *slaveid, uid_local = *uniqueid;

	if (!addresses_load(&uid_local, &sid_local)) {
		leds_on();
	}

	*slaveid = sid_local;
	*uniqueid = uid_local;
}

uint8_t address_valid(uint8_t uniqueid, uint8_t slaveid) {
	uint8_t mem_unique, mem_slave, mem_sum, mem_ucrc, mem_scrc, mem_bothcrc;
	uint8_t sum, ucrc, scrc, bothcrc;

	for (uint8_t startval = ANFANG_ID_SPEICHER; startval < (ANFANG_ID_SPEICHER + 3 * STEP_ID_SPEICHER);
			startval += STEP_ID_SPEICHER) {

		// Werte aus dem Speicher
		mem_unique = eeread(startval);
		mem_slave = eeread(startval + 1);
		mem_sum = eeread(startval + 2);		// Summe der beiden IDs
		mem_ucrc = eeread(startval + 3);		// CRC8-Wert der Unique-ID mit Startwert 16
		mem_scrc = eeread(startval + 4);		// CRC8-Wert der Slave-ID mit Startwert 16
		mem_bothcrc = eeread(startval + 5);	// CRC8-Wert aus Unique-ID und Slave-ID mit Startwert 16

		// Werte aus �bergabe
		sum = uniqueid + slaveid;			// Summe der beiden IDs
		ucrc = crc8(CRC_ID_SPEICHER, uniqueid);			// CRC8-Wert der Unique-ID mit Startwert 16
		scrc = crc8(CRC_ID_SPEICHER, slaveid);			// CRC8-Wert der Slave-ID mit Startwert 16
		bothcrc = crc8(ucrc, slaveid);		// CRC8-Wert aus Unique-ID und Slave-ID mit Startwert 16

		// Vergleichen
		if ((mem_unique == uniqueid) && (mem_slave == slaveid) && (mem_sum == sum) && (mem_ucrc == ucrc)
				&& (mem_scrc == scrc) && (mem_bothcrc == bothcrc)) return 1;
	}
	return 0;
}

void address_get(uint8_t *uid, uint8_t *sid, uint8_t times) {
	uint8_t uid_local = *uid;
	uint8_t sid_local = *sid;

	uid_local = eeread(ANFANG_ID_SPEICHER + times * STEP_ID_SPEICHER);
	sid_local = eeread(ANFANG_ID_SPEICHER + 1 + times * STEP_ID_SPEICHER);

	*uid = uid_local;
	*sid = sid_local;
}

uint8_t addresses_load(uint8_t *uniqueid, uint8_t *slaveid) {
	uint8_t sid_local = *slaveid, uid_local = *uniqueid;

	for (uint8_t i = 0; i < 3; i++) {										// Im EEPROM nach IDs suchen
		address_get(&uid_local, &sid_local, i);							// Aus dem Speicher holen

		if (address_valid(uid_local, sid_local)) {
			if (ID_MESS) savenumber(uid_local, sid_local); 	// Neu speichern, wenn Speicherstellen ungleich
			*uniqueid = uid_local;
			*slaveid = sid_local;
			return 1;
		}

		if ((i == 2) && !(address_valid(uid_local, sid_local))) {// 0 zur�ckgeben, wenn alle Bl�cke fehlerhaft
			*uniqueid = 'E';
			*slaveid = 'e';
			return 0;
		}
	}
	return 0;
}
