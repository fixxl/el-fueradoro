/*
 * terminal.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

void terminal_reset(void) {
	uart_puts_P(PSTR("\033[2J"));
	uart_puts_P(PSTR("\033[0;0H"));
	uart_puts_P(PSTR("\033[3J"));
	uart_puts_P(PSTR("\033]0;EL FUERADORO\007"));
}

void fixedspace(int32_t zahl, uint8_t type, uint8_t space) {
	uint8_t cntr = 0;
	int32_t num_temp = zahl;

	while (num_temp) {
		num_temp /= 10;
		cntr++;
	}
	if ((zahl < 0) && space) space--;
	if (space >= cntr) {
		space -= cntr;
		for (uint8_t i = 0; i < space; i++) {
			uart_puts_P(PSTR(" "));
		}
	}
	uart_shownum(zahl, type);
}

uint8_t changenumber(void) {
	uint8_t zehner = 'c', einer = 'c', number;
	while (!(zehner >= '0' && zehner <= '3'))
		zehner = uart_getc();
	uart_putc(zehner);
	if (zehner == '0') while (!(einer >= '1' && einer <= '9'))
		einer = uart_getc();
	if (zehner == '1' || zehner == '2') while (!(einer >= '0' && einer <= '9'))
		einer = uart_getc();
	if (zehner == '3') while (einer != '0')
		einer = uart_getc();
	uart_putc(einer);
	number = (zehner - '0') * 10 + (einer - '0');
	return number;
}

void savenumber(uint8_t uniqueid, uint8_t slaveid) {
	uint8_t sum = uniqueid + slaveid;		// Summe der beiden IDs
	uint8_t ucrc = crc8(CRC_ID_SPEICHER, uniqueid);		// CRC8-Wert der Unique-ID mit Startwert 16
	uint8_t scrc = crc8(CRC_ID_SPEICHER, slaveid);		// CRC8-Wert der Slave-ID mit Startwert 16
	uint8_t bothcrc = crc8(ucrc, slaveid);	// CRC8-Wert aus Unique-ID und Slave-ID mit Startwert 16

	for (uint8_t i = ANFANG_ID_SPEICHER; i < (ANFANG_ID_SPEICHER + 3 * STEP_ID_SPEICHER); i +=
	STEP_ID_SPEICHER) {
		eewrite(uniqueid, i);
		eewrite(slaveid, i + 1);
		eewrite(sum, i + 2);
		eewrite(ucrc, i + 3);
		eewrite(scrc, i + 4);
		eewrite(bothcrc, i + 5);
	}
	if (address_valid(uniqueid, slaveid)) {
		uart_puts_P(PSTR("\n\n\rÄnderung erfolgreich übernommen!\n\r"));
	}
	else {
		uart_puts_P(PSTR("Ein Fehler ist aufgetreten. Bitte erneut versuchen!\n\r"));
	}
}

// Konfigurationsprogramm
uint8_t configprog(void) {
	uint8_t changes = 0;						// Merker, ob Änderungen vorgenommen wurden
	uint8_t choice = 0;							// Tastatureingabe
	uint8_t slaveid;							// Slave-
	uint8_t uniqueid;							// und Unique-ID
	uint8_t slaveid_old;
	uint8_t uniqueid_old;

	terminal_reset();

	uart_puts_P(PSTR(TERM_COL_YELLOW));
	uart_puts_P(PSTR("\n\nKonfigurationsprogramm\n\r======================\n\r"));

	if (!addresses_load(&uniqueid, &slaveid)) {	// Bisherige Adressen aus dem Speicher holen und prüfen
		uart_puts_P(PSTR(TERM_COL_RED));
		uart_puts_P(PSTR("\n\rFehler beim Laden der IDs\n\r"));
		uart_puts_P(PSTR(TERM_COL_WHITE));
	}

	// Anzeige der bisherigen Einstellungen
	uart_puts_P(PSTR(TERM_COL_WHITE));
	uart_puts_P(PSTR("Aktuelle Unique-ID: "));
	uart_puts_P(PSTR(TERM_COL_RED));
	if (uniqueid != 'E') uart_shownum(uniqueid, 'd');
	else uart_puts_P(PSTR("FEHLER"));
	uart_puts_P(PSTR("\n\r"));
	uart_puts_P(PSTR(TERM_COL_WHITE));
	uart_puts_P(PSTR("Aktuelle Slave-ID:  "));
	uart_puts_P(PSTR(TERM_COL_RED));
	if (slaveid != 'e') uart_shownum(slaveid, 'd');
	else uart_puts_P(PSTR("FEHLER"));
	uart_puts_P(PSTR("\n\r"));
	uart_puts_P(PSTR(TERM_COL_WHITE));
	if ((slaveid == uniqueid) && !slaveid)
		uart_puts_P(PSTR("\n\n\rDevice ist derzeit als Transmitter konfiguriert."));
	uart_puts_P(PSTR("\n\n\n\r"));
	uart_puts_P(
			PSTR(
					"(U)nique-ID ändern, (S)lave-ID ändern, \n\rzu (T)ransmitter machen, Abbruch mit beliebiger anderer Taste! "));

	// Benutzereingabe auswerten
	while(!choice) {
		choice = uart_getc();
	}
	uart_putc(choice);
	uart_puts_P(PSTR("\n\r"));

	do {
		uart_puts_P(PSTR("\n\r"));
		switch (choice) {
			case 's':
			case 'S': {
				uart_puts_P(PSTR("Neue Slave-ID (01-30):  "));
				uart_puts_P(PSTR(TERM_COL_RED));
				slaveid_old = slaveid;
				slaveid = changenumber();
				uart_puts_P(PSTR(TERM_COL_WHITE));
				choice = 'u';
				if(slaveid != slaveid_old) changes = 1;
				break;
			}
			case 'u':
			case 'U': {
				uart_puts_P(PSTR("Neue Unique-ID (01-30): "));
				uart_puts_P(PSTR(TERM_COL_RED));
				uniqueid_old = uniqueid;
				uniqueid = changenumber();
				uart_puts_P(PSTR(TERM_COL_WHITE));
				choice = 's';
				if(uniqueid != uniqueid_old) changes = 1;
				break;
			}
			case 't':
			case 'T': {
				if(slaveid && uniqueid) uart_puts_P(PSTR("Device ist jetzt Transmitter!\n\r"));
				slaveid_old = slaveid;
				uniqueid_old = uniqueid;
				slaveid = 0;
				uniqueid = 0;
				if(uniqueid_old || slaveid_old) changes = 1;
				break;
			}
			default:
				break;
		}
	} while ((slaveid != 0) ^ (uniqueid != 0));

	if (changes) {
		savenumber(uniqueid, slaveid);
		uart_puts_P(PSTR("\n\rNeustart...\n\n\r"));
	}
	else {
		uart_puts_P(PSTR("\n\rKeine Änderungen vorgenommen\n\n\r"));
	}
	return changes;
}

// Auflisten der Zündboxen
void list_complete(char *boxe, char *batt, char *sharpn, int8_t* temps, int8_t* rssis, uint8_t wrongids) {
	uint8_t i = 0, ganz, zehntel;

	terminal_reset();
	uart_puts_P(PSTR(TERM_COL_YELLOW));
	uart_puts_P(PSTR("\n\n\rSystemübersicht\n\r"));
	uart_puts_P(PSTR("===============\n\r"));

	uart_puts_P(PSTR(TERM_COL_WHITE));
	uart_puts_P(
			PSTR(
					"\n\n\rUnique-ID: Slave-ID, Batteriespannung (V), Scharf?, Temperatur (°C), RSSI (dBm)\n\r"));
	while (i < 30) {
		// Unique-ID darstellen
		if (i < 9) uart_putc('0');
		uart_shownum(i + 1, 'd');
		uart_puts_P(PSTR(": "));

		// Slave-ID ausgeben
		switch (boxe[i]) {
			case 0:
				uart_puts_P(PSTR("---"));
				break;
			default: {
				if (boxe[i] < 100) uart_puts_P(PSTR(" "));
				if (boxe[i] < 10) uart_puts_P(PSTR("0"));
				uart_shownum(boxe[i], 'd');
				break;
			}
		}

		uart_puts_P(PSTR(", "));

		// Batteriespannung ausgeben
		ganz = batt[i] / 10;
		zehntel = batt[i] % 10;
		switch (ganz) {
			case 0:
				uart_puts_P(PSTR("----"));
				break;
			default: {
				fixedspace(ganz, 'd', 2);
				uart_puts_P(PSTR("."));
				uart_shownum(zehntel, 'd');
				break;
			}
		}

		uart_puts_P(PSTR(", "));

		// Ausgeben, ob Box scharf oder nicht
		if (boxe[i]) uart_putc(sharpn[i]);
		else uart_puts_P(PSTR("-"));

		uart_puts_P(PSTR(", "));

		// Temperatur ausgeben
		if (temps[i] != -128) {
			fixedspace(temps[i], 'd', 4);
		}
		else {
			boxe[i] ? uart_puts_P(PSTR("n.a.")) : uart_puts_P(PSTR("----"));
		}

		uart_puts_P(PSTR(", "));

		// RSSI ausgeben
		if (rssis[i]) {
			if (rssis[i] < 100) uart_puts_P(PSTR(" -"));
			else uart_puts_P(PSTR("-"));
			uart_shownum(rssis[i], 'd');
		}
		else uart_puts_P(PSTR("----"));

		if ((i % 3) == 2) uart_puts_P(PSTR("\n\r"));
		else uart_puts_P(PSTR("     "));
		i++;
	}
	uart_puts_P(PSTR("\n\r\n\r"));
	if(wrongids) uart_puts_P(PSTR(TERM_COL_RED));
	uart_puts_P(PSTR("Fehlerhafte IDs: "));
	uart_shownum(wrongids, 'd');
	if(wrongids) uart_puts_P(PSTR(TERM_COL_WHITE));
	uart_puts_P(PSTR("\n\r"));
}

// Anzahl der Boxen mit bestimmter Slave-ID darstellen
void list_array(char *arr) {
	uint8_t i = 0;
	uart_puts_P(PSTR("\n\n\rSlave-ID: Anzahl Boxen\n\r"));
	while (i < 30) {
		if (i < 9) uart_putc('0');
		uart_shownum(i + 1, 'd');
		uart_puts_P(PSTR(": "));
		switch (arr[i]) {
			case 0:
				uart_puts_P(PSTR("---"));
				break;
			default: {
				fixedspace(arr[i], 'd', 3);
				break;
			}
		}

		if ((i % 3) == 2) {
			uart_puts_P(PSTR("\n\r"));
		}
		else {
			for (uint8_t laufvariable = 0; laufvariable < 26; laufvariable++) {
				uart_puts_P(PSTR(" "));
			}
		}
		i++;
	}
	uart_puts_P(PSTR("\n\r"));
}

// Anzahl der Boxen mit bestimmter Slave-ID ermitteln
void evaluate_boxes(char *boxes, char *quantity) {
	uint8_t i, j, n;

	for (i = 1; i < 31; i++) {
		n = 0;
		for (j = 0; j < 30; j++) {
			if (boxes[j] == i) n++;
		}
		quantity[i - 1] = n;
	}
}
