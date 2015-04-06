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

// Print number and put spaces in front to achieve a defined length
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
		for (uint8_t i = space; i; i--) {
			uart_puts_P(PSTR(" "));
		}
	}
	uart_shownum(zahl, type);
}

// GUI-Routine to change IDs (allows numbers from 01 to 30)
static uint8_t changenumber(void) {
	uint8_t zehner = 'c', einer = 'c', number = 0;
	while (!(zehner >= '0' && zehner <= '3')) {
		zehner = uart_getc();
		if (zehner == 10 || zehner == 13) return 255;
	}
	uart_putc(zehner);
	while (!(number > 0 && number < 31)) {
		einer = uart_getc();
		if (einer == 10 || einer == 13) return 255;
		number = (zehner - '0') * 10 + (einer - '0');
	}
	uart_putc(einer);
	return number;
}

// Remote configuration
uint8_t remote_config(char* txf) {
	uint8_t valid = 0;
	char temporary[5];

	uart_puts_P(PSTR(TERM_COL_YELLOW));
	uart_puts_P(PSTR("\n\nRemote-Konfigurationsprogramm\n\r=============================\n\r"));

	uart_puts_P(PSTR(TERM_COL_WHITE));
	uart_puts_P(PSTR("Bisherige Unique-ID: "));
	temporary[0] = changenumber();
	uart_puts_P(PSTR("\n\r"));
	if (temporary[0] == 255) return 0;

	uart_puts_P(PSTR("Bisherige Slave-ID:  "));
	temporary[1] = changenumber();
	uart_puts_P(PSTR("\n\r"));
	uart_puts_P(PSTR("\n\r"));
	if (temporary[1] == 255) return 0;

	uart_puts_P(PSTR("Neue Unique-ID:      "));
	temporary[2] = changenumber();
	uart_puts_P(PSTR("\n\r"));
	if (temporary[2] == 255) return 0;

	uart_puts_P(PSTR("Neue Slave-ID:       "));
	temporary[3] = changenumber();
	uart_puts_P(PSTR("\n\n\r"));
	if (temporary[3] == 255) return 0;

	if ((temporary[0] != temporary[2]) || (temporary[1] != temporary[3])) {
		uart_puts_P(PSTR("ID-Wechsel mit j bestätigen, abbrechen mit anderer Taste! "));
		while (!valid) {
			valid = uart_getc();
		}
		uart_putc(valid);
		if ((valid | 0x20) == 'j') {
			txf[0] = CHANGE;
			for (uint8_t i = 0; i < 4; i++) {
				txf[i + 1] = temporary[i];
			}
			valid = 1;
		}
		else {
			valid = 0;
		}
	}
	return valid;
}

// Configuration programme
uint8_t configprog(const uint8_t devicetype) {
	uint8_t changes = 0; 	// Merker, ob Änderungen vorgenommen wurden
	uint8_t choice = 0; 	// Tastatureingabe
	uint8_t slaveid; 		// Slave-
	uint8_t uniqueid; 		// und Unique-ID
	uint8_t slaveid_old;
	uint8_t uniqueid_old;

	terminal_reset();

	uart_puts_P(PSTR(TERM_COL_YELLOW));
	uart_puts_P(PSTR("\nKonfigurationsprogramm\n\r======================\n\r"));

	if (!addresses_load(&uniqueid, &slaveid)) { // Bisherige Adressen aus dem Speicher holen und prüfen
		uart_puts_P(PSTR(TERM_COL_RED));
		uart_puts_P(PSTR("\n\rFehler beim ID-Laden\n\r"));
		uart_puts_P(PSTR(TERM_COL_WHITE));
	}

	// Show current settings
	uart_puts_P(PSTR(TERM_COL_WHITE));
	uart_puts_P(PSTR("\n\rAngeschlossenes Device: "));
	uart_puts(devicetype ? "Zündbox" : "Transmitter");

	if (devicetype) {
		uart_puts_P(PSTR("\n\n\rAktuelle Unique-ID: "));
		uart_puts_P(PSTR(TERM_COL_RED));
		if (uniqueid != 'E') {
			if (uniqueid < 10) uart_putc('0');
			uart_shownum(uniqueid, 'd');
		}
		else {
			uart_puts_P(PSTR("FEHLER"));
		}
		uart_puts_P(PSTR("\n\r"));
		uart_puts_P(PSTR(TERM_COL_WHITE));
		uart_puts_P(PSTR("Aktuelle Slave-ID:  "));
		uart_puts_P(PSTR(TERM_COL_RED));
		if (slaveid != 'e') {
			if (slaveid < 10) uart_putc('0');
			uart_shownum(slaveid, 'd');
		}
		else {
			uart_puts_P(PSTR("FEHLER"));
		}
		uart_puts_P(PSTR("\n\r"));
		uart_puts_P(PSTR(TERM_COL_WHITE));
		uart_puts_P(PSTR("\n\n\n\r"));
		uart_puts_P(PSTR("(I)Ds ändern, Abbruch mit beliebiger anderer Taste! "));

		// Evaluate input
		while (!choice) {
			choice = uart_getc();
		}
		choice |= 0x20;
		uart_putc(choice);
		uart_puts_P(PSTR("\n\r"));

		uart_puts_P(PSTR("\n\r"));
		switch (choice) {
			case 'i': {
				uart_puts_P(PSTR("Neue Unique-ID (01-30, ENTER = alter Wert): "));
				uart_puts_P(PSTR(TERM_COL_RED));
				uniqueid_old = uniqueid;
				uniqueid = changenumber();
				if (uniqueid == 255) uniqueid = uniqueid_old;
				uart_puts_P(PSTR(TERM_COL_WHITE));

				uart_puts_P(PSTR("\n\rNeue Slave-ID (01-30, ENTER = alter Wert):  "));
				uart_puts_P(PSTR(TERM_COL_RED));
				slaveid_old = slaveid;
				slaveid = changenumber();
				if (slaveid == 255) slaveid = slaveid_old;
				uart_puts_P(PSTR(TERM_COL_WHITE));

				if (((uniqueid != uniqueid_old) || (slaveid != slaveid_old))) changes = 1;
				break;
			}
			default: {
				break;
			}
		}

		if (changes) {
			addresses_save(uniqueid, slaveid);
			if (address_valid(uniqueid, slaveid)) {
				uart_puts_P(PSTR("\n\n\rÄnderung erfolgreich!\n\r"));
			}
			else {
				uart_puts_P(PSTR("Fehler! Bitte erneut versuchen!\n\r"));
			}
			uart_puts_P(PSTR("\n\rNeustart...\n\n\r"));
		}
		else {
			uart_puts_P(PSTR("\n\rKeine Änderung vorgenommen\n\n\r"));
		}

	}
	else {
		uart_puts_P(PSTR("\n\rDevice ist Transmitter, IDs nicht änderbar!\n\n\r"));
	}
	return changes;
}

// List ignition devices
void list_complete(char *slvs, char *batt, char *sharpn, int8_t* temps, int8_t* rssis, uint8_t wrongids) {
	uint8_t i = 0, ganz, zehntel;

	terminal_reset();
	uart_puts_P(PSTR(TERM_COL_YELLOW));
	uart_puts_P(PSTR("\n\rSystemübersicht\n\r"));
	uart_puts_P(PSTR("===============\n\r"));

	uart_puts_P(PSTR(TERM_COL_WHITE));
	uart_puts_P(
			PSTR("\n\rUnique-ID: Slave-ID, Batteriespannung (V), Scharf?, Temperatur (°C), RSSI (dBm)\n\r"));
	while (i < 30) {
		// Show Unique-ID
		if ((i + 1) < 10) uart_puts_P(PSTR("0"));
		uart_shownum(i + 1, 'd');
		uart_puts_P(PSTR(": "));

		// Show Slave-ID
		if (!slvs[i]) {
			uart_puts_P(PSTR("---"));
		}
		else {
			uart_puts_P(PSTR(" "));
			if (slvs[i] < 10) uart_puts_P(PSTR("0"));
			uart_shownum(slvs[i], 'd');
		}

		uart_puts_P(PSTR(", "));

		// Show Battery Voltages
		ganz = batt[i] / 10;
		zehntel = batt[i] % 10;
		if (!ganz) {
			uart_puts_P(PSTR("----"));
		}
		else {
			fixedspace(ganz, 'd', 2);
			uart_puts_P(PSTR("."));
			uart_shownum(zehntel, 'd');
		}

		uart_puts_P(PSTR(", "));

		// Show if armed or not
		if (slvs[i]) uart_putc(sharpn[i]);
		else uart_puts_P(PSTR("-"));

		uart_puts_P(PSTR(", "));

		// Show Temperature
		if (temps[i] != -128) {
			fixedspace(temps[i], 'd', 4);
		}
		else {
			slvs[i] ? uart_puts_P(PSTR("n.a.")) : uart_puts_P(PSTR("----"));
		}

		uart_puts_P(PSTR(", "));

		// Show RSSI-values
		if (rssis[i]) {
			if (rssis[i] < 100) uart_puts_P(PSTR(" -"));
			else uart_puts_P(PSTR("-"));
			uart_shownum(rssis[i], 'd');
		}
		else uart_puts_P(PSTR("----"));

		if ((i % 3) == 2) uart_puts_P(PSTR("\n\r"));
		else uart_puts_P(PSTR("\t"));
		i++;
	}
	uart_puts_P(PSTR("\n\rFehlerhafte/doppelte IDs: "));
	uart_shownum(wrongids, 'd');
	uart_puts_P(PSTR("\n\r"));
}

// Show number of boxes for every Slave-ID
void list_array(char *arr) {
	uint8_t i = 0;
	uart_puts_P(PSTR("\n\rSlave-ID: Anzahl Boxen\n\r"));
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
			uart_puts_P(PSTR("\t \t \t \t"));
		}
		i++;
	}
	uart_puts_P(PSTR("\n\n\r"));
}

// Calculate number of boxes with certain Slave-ID
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
