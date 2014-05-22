/*
 * addresses.c
 *
 *  Created on: 06.11.2013
 *      Author: Felix
 */

#include "global.h"

// Prüfroutine, ob Sender- oder Zündbox
void update_addresses(uint8_t *uniqueid, uint8_t *slaveid) {
	uint8_t sid_local = *slaveid, uid_local = *uniqueid;

	if (!addresses_load(&uid_local, &sid_local)) {
		uart_puts_P(PSTR(TERM_COL_RED));
		uart_puts_P(PSTR("\n\rFehler beim Laden der IDs\n\r"));
		uart_puts_P(PSTR(TERM_COL_WHITE));
	}

	// Senderspezifische Initialisierung
	if ((sid_local == 0) && (uid_local == 0)) {
		clear_lcd_rx_flag = 0;
		clear_lcd_tx_flag = 0;
		hist_del_flag = 0;
		key_deinit();
		key_flag = 0;
		leds_off();
		lcd_init();
		create_symbols();
		lcd_cursorset(2, 1);
		lcd_send(0, 1);
		lcd_send(0, 1);
		lcd_puts("  El Fueradoro  ");
		lcd_send(0, 1);
		lcd_send(0, 1);
		lcd_cursorhome();
		TCCR0B |= (1 << CS02 | 1 << CS00);
		TIMSK0 |= (1 << TOIE0);
		_delay_ms(2500);
		lcd_clear();
	}

	// Empfängerspezifische Initialisierung
	else {
		sr_init();
		key_init();
		key_flag = 1;
		adc_init();
		leds_off();
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

		// Werte aus Übergabe
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

		if ((i == 2) && !(address_valid(uid_local, sid_local))) {// 0 zurückgeben, wenn alle Blöcke fehlerhaft
			*uniqueid = 'E';
			*slaveid = 'e';
			return 0;
		}
	}
	return 0;
}
