/*
 * addresses.c
 *
 *  Created on: 06.11.2013
 *      Author: Felix
 */

#include "global.h"

// Get addresses and signalize failure if unsuccessful or conditions not met
void update_addresses(uint8_t *uniqueid, uint8_t *slaveid) {
	uint8_t sid_local = *slaveid, uid_local = *uniqueid;

	if (!addresses_load(&uid_local, &sid_local)) {
		leds_on();
	}

	*slaveid = sid_local;
	*uniqueid = uid_local;
}

// Control if addresses are valid
uint8_t address_valid(uint8_t uniqueid, uint8_t slaveid) {
	uint8_t mem_unique, mem_slave, mem_sum, mem_ucrc, mem_scrc, mem_bothcrc;
	uint8_t sum, ucrc, scrc, bothcrc;

	for (uint8_t startval = START_ADDRESS_ID_STORAGE;
			startval < (START_ADDRESS_ID_STORAGE + 3 * STEP_ID_STORAGE); startval += STEP_ID_STORAGE) {

		// Get values from eeprom
		mem_unique = eeread(startval);
		mem_slave = eeread(startval + 1);
		mem_sum = eeread(startval + 2);			// Sum of both IDs
		mem_ucrc = eeread(startval + 3);		// CRC8-value of Unique-ID (seed = 16)
		mem_scrc = eeread(startval + 4);		// CRC8-value of Slave-ID (seed 16)
		mem_bothcrc = eeread(startval + 5);		// CRC8-value of Unique-ID and Slave-ID (seed 16)

		// Get values from function call
		sum = uniqueid + slaveid;				// Summe of both IDs
		ucrc = crc8(CRC_ID_STORAGE, uniqueid);	// CRC8-value of Unique-ID (seed = 16)
		scrc = crc8(CRC_ID_STORAGE, slaveid);	// CRC8-value of Slave-ID (seed 16)
		bothcrc = crc8(ucrc, slaveid);			// CRC8-value of Unique-ID and Slave-ID (seed 16)

		// Compare values and return 1 if everything is fine
		if ((mem_unique == uniqueid) && (mem_slave == slaveid) && (mem_sum == sum) && (mem_ucrc == ucrc)
				&& (mem_scrc == scrc) && (mem_bothcrc == bothcrc)) return 1;
	}
	// Return 0 if all 3 tries have failed
	return 0;
}

// Read IDs from storage position 0, 1 or 2
static void address_get(uint8_t *uid, uint8_t *sid, uint8_t storage_position) {
	uint8_t uid_local = *uid;
	uint8_t sid_local = *sid;

	uid_local = eeread(START_ADDRESS_ID_STORAGE + storage_position * STEP_ID_STORAGE);
	sid_local = eeread(START_ADDRESS_ID_STORAGE + 1 + storage_position * STEP_ID_STORAGE);

	*uid = uid_local;
	*sid = sid_local;
}

// Load addresses and make sure, they're stored correctly in all 3 places
uint8_t addresses_load(uint8_t *uniqueid, uint8_t *slaveid) {
	uint8_t sid_local = *slaveid, uid_local = *uniqueid;

	for (uint8_t i = 0; i < 3; i++) {						// Try up to three times (three storage places)
		address_get(&uid_local, &sid_local, i);						// Read from memory

		if (address_valid(uid_local, sid_local)) {					// If valid numbers are found
			if (ID_MESS) addresses_save(uid_local, sid_local); // Check if all storage positions are correct, rewrite if not
			*uniqueid = uid_local;
			*slaveid = sid_local;
			return 1;
		}

		if ((i == 2) && !(address_valid(uid_local, sid_local))) {	// Return 0 if all tries failed
			*uniqueid = 'E';
			*slaveid = 'e';
			return 0;
		}
	}
	return 0;
}

// Save IDs
void addresses_save(uint8_t uniqueid, uint8_t slaveid) {
	uint8_t sum = uniqueid + slaveid; // Summe der beiden IDs
	uint8_t ucrc = crc8(CRC_ID_STORAGE, uniqueid); // CRC8-Wert of Unique-ID with seed 16
	uint8_t scrc = crc8(CRC_ID_STORAGE, slaveid); // CRC8-Wert of Slave-ID with seed 16
	uint8_t bothcrc = crc8(ucrc, slaveid); // CRC8-Wert of Unique-ID and Slave-ID with seed 16

	for (uint8_t i = START_ADDRESS_ID_STORAGE; i < (START_ADDRESS_ID_STORAGE + 3 * STEP_ID_STORAGE); i +=
	STEP_ID_STORAGE) {
		eewrite(uniqueid, i);
		eewrite(slaveid, i + 1);
		eewrite(sum, i + 2);
		eewrite(ucrc, i + 3);
		eewrite(scrc, i + 4);
		eewrite(bothcrc, i + 5);
	}
}
