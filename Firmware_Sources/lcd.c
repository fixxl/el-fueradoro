/*
 * lcdbib.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// Apply data
static void lcd_enable(void) {
	E_HIGH;
	asm volatile ("rjmp 1f\n 1:");
	asm volatile ("rjmp 1f\n 1:");
	E_LOW;
}

// Wait for busy flag
static void lcd_busycheck(void) {
	lcd_cursorread();
}

// Clear data bits
static void lcd_clear_all_bits(void) {
	#if (LCD_BIT_MODE == 8)
		DB0PORT &= ~(1 << DB0);
		DB1PORT &= ~(1 << DB1);
		DB2PORT &= ~(1 << DB2);
		DB3PORT &= ~(1 << DB3);
	#endif

	DB4PORT &= ~(1 << DB4);
	DB5PORT &= ~(1 << DB5);
	DB6PORT &= ~(1 << DB6);
	DB7PORT &= ~(1 << DB7);
}

// Set data bits
static void lcd_set_all_bits() {
	#if (LCD_BIT_MODE == 8)
		DB0PORT |= (1 << DB0);
		DB1PORT |= (1 << DB1);
		DB2PORT |= (1 << DB2);
		DB3PORT |= (1 << DB3);
	#endif

	DB4PORT |= (1 << DB4);
	DB5PORT |= (1 << DB5);
	DB6PORT |= (1 << DB6);
	DB7PORT |= (1 << DB7);
}

// Transfer byte to LCD bit by bit
static void lcd_transfer(uint8_t data) {
	lcd_clear_all_bits();

	#if (LCD_BIT_MODE == 8)
		if (data & 1) DB0PORT |= 1 << DB0;

		if (data & 2) DB1PORT |= 1 << DB1;

		if (data & 4) DB2PORT |= 1 << DB2;

		if (data & 8) DB3PORT |= 1 << DB3;

		if (data & 16) DB4PORT |= 1 << DB4;

		if (data & 32) DB5PORT |= 1 << DB5;

		if (data & 64) DB6PORT |= 1 << DB6;

		if (data & 128) DB7PORT |= 1 << DB7;
		lcd_enable();
	#endif

	#if (LCD_BIT_MODE != 8)
		if (data & 16) DB4PORT |= 1 << DB4;

		if (data & 32) DB5PORT |= 1 << DB5;

		if (data & 64) DB6PORT |= 1 << DB6;

		if (data & 128) DB7PORT |= 1 << DB7;

		lcd_enable();
		lcd_clear_all_bits();

		if (data & 1) DB4PORT |= 1 << DB4;

		if (data & 2) DB5PORT |= 1 << DB5;

		if (data & 4) DB6PORT |= 1 << DB6;

		if (data & 8) DB7PORT |= 1 << DB7;
		lcd_enable();
	#endif
}

// Write data to CGRAM (e.g. symbol definitions)
void lcd_cgwrite(uint8_t data) {
	SCHREIBEN;
	DATENMODUS;

	lcd_transfer(data);

	lcd_busycheck();
}

// Send data or order to LCD
void lcd_send(uint8_t data, uint8_t dat) {
	SCHREIBEN;

	// Daten- (1) oder Befehlsmodus (0)
	if (!dat) BEFEHLSMODUS;

	if (dat) DATENMODUS;

	lcd_transfer(data);

	lcd_busycheck();

	if (dat) {
		uint8_t posi = lcd_cursorread();

		switch (posi) {
			 case (COLUMNS): {
				  lcd_cursorset(2, 1);
				  break;
			  }

			 case (64 + COLUMNS): {
				  lcd_cursorset(3, 1);
				  break;
			  }

			 case (64): {
				  lcd_cursorset(4, 1);
				  break;
			  }

			 case (64 + 2 * COLUMNS): {
				  lcd_cursorset(1, 1);
				  break;
			  }

			 default: {
				  break;
			  }
		}
	}
}

// Clear LCD screen
void lcd_clear(void) {
	lcd_send(1 << 0, 0);
}

// Set cursor to first line and first column
void lcd_cursorhome(void) {
	lcd_send(1 << 1, 0);
}

// Read-out address pins
static uint8_t lcd_getaddr() {
	uint8_t addr = 0;

	E_HIGH;
	asm volatile ("nop");
	asm volatile ("nop");
	addr |= (DB4PIN & (1 << DB4)) ? 16 : 0;
	addr |= (DB5PIN & (1 << DB5)) ? 32 : 0;
	addr |= (DB6PIN & (1 << DB6)) ? 64 : 0;
	addr |= (DB7PIN & (1 << DB7)) ? 128 : 0;
	#if (LCD_BIT_MODE == 8)
		addr |= (DB0PIN & (1 << DB0)) ? 1 : 0;
		addr |= (DB1PIN & (1 << DB1)) ? 2 : 0;
		addr |= (DB2PIN & (1 << DB2)) ? 4 : 0;
		addr |= (DB3PIN & (1 << DB3)) ? 8 : 0;
	#endif
	E_LOW;

	#if (LCD_BIT_MODE != 8)
		asm volatile ("nop");
		asm volatile ("nop");
		E_HIGH;
		asm volatile ("nop");
		asm volatile ("nop");
		addr |= (DB4PIN & (1 << DB4)) ? 1 : 0;
		addr |= (DB5PIN & (1 << DB5)) ? 2 : 0;
		addr |= (DB6PIN & (1 << DB6)) ? 4 : 0;
		addr |= (DB7PIN & (1 << DB7)) ? 8 : 0;
		E_LOW;
	#endif

	return addr;
}

// Read current cursor position
uint8_t lcd_cursorread() {
	uint8_t addr = 0x80;
	uint16_t utimer = 10000;

	// Datenleitungswerte zwischenspeichern
	uint8_t zwsp = 0;

	#if (LCD_BIT_MODE == 8)
		zwsp |= (DB0PIN & (1 << DB0)) ? 1 : 0;
		zwsp |= (DB1PIN & (1 << DB1)) ? 2 : 0;
		zwsp |= (DB2PIN & (1 << DB2)) ? 4 : 0;
		zwsp |= (DB3PIN & (1 << DB3)) ? 8 : 0;
	#endif

	zwsp |= (DB4PIN & (1 << DB4)) ? 16 : 0;
	zwsp |= (DB5PIN & (1 << DB5)) ? 32 : 0;
	zwsp |= (DB6PIN & (1 << DB6)) ? 64 : 0;
	zwsp |= (DB7PIN & (1 << DB7)) ? 128 : 0;

	// Ausgänge high
	lcd_set_all_bits();

	// Datenleitungen werden Eingänge mit akt. Pullups
	#if (LCD_BIT_MODE == 8)
		DB0DDR &= ~(1 << DB0);
		DB1DDR &= ~(1 << DB1);
		DB2DDR &= ~(1 << DB2);
		DB3DDR &= ~(1 << DB3);
	#endif
	DB4DDR &= ~(1 << DB4);
	DB5DDR &= ~(1 << DB5);
	DB6DDR &= ~(1 << DB6);
	DB7DDR &= ~(1 << DB7);

	BEFEHLSMODUS;
	LESEN;

	while (--utimer && (addr & (1 << 7))) addr = lcd_getaddr();

	SCHREIBEN;

	// Datenleitungen werden Ausgänge
	#if (LCD_BIT_MODE == 8)
		DB0DDR |= (1 << DB0);
		DB1DDR |= (1 << DB1);
		DB2DDR |= (1 << DB2);
		DB3DDR |= (1 << DB3);
	#endif
	DB4DDR |= (1 << DB4);
	DB5DDR |= (1 << DB5);
	DB6DDR |= (1 << DB6);
	DB7DDR |= (1 << DB7);

	lcd_clear_all_bits();

	// Datenleitungswerte von vor der Abfrage widerherstellen
	#if (LCD_BIT_MODE == 8)
		if (zwsp & 1) DB0PORT |= 1 << DB0;

		if (zwsp & 2) DB1PORT |= 1 << DB1;

		if (zwsp & 4) DB2PORT |= 1 << DB2;

		if (zwsp & 8) DB3PORT |= 1 << DB3;
	#endif

	if (zwsp & 16) DB4PORT |= 1 << DB4;

	if (zwsp & 32) DB5PORT |= 1 << DB5;

	if (zwsp & 64) DB6PORT |= 1 << DB6;

	if (zwsp & 128) DB7PORT |= 1 << DB7;

	return addr & 0x7F;
}

// Set cursor position
void lcd_cursorset(uint8_t zeile, uint8_t spalte) {
	uint8_t numbr;

	if (zeile > LINES) zeile = 1;

	if (spalte > COLUMNS) spalte = 1;

	uint8_t zmult  = (1 - (zeile & 1));
	uint8_t spmult = ((zeile - 1) >> 1);
	numbr = ((1 << 7) + (zmult << 6) + (spmult << 4) + (spmult << 2) + spalte - 1);

	SCHREIBEN;
	BEFEHLSMODUS;

	lcd_transfer(numbr);

	lcd_busycheck();
}

// Display String
void lcd_puts(char *strin) {
	while (*strin) lcd_send(*strin++, 1);
}

// Print number to array
void lcd_arrize(int32_t zahl, char *feld, uint8_t digits, uint8_t vorzeichen) {
	uint8_t neededlength = 1;

	if (vorzeichen || (zahl < 0)) {
		vorzeichen = 1;
		feld[0]    = (zahl < 0) ? '-' : '+';

		if (zahl < 0) zahl = -zahl;
	}

	if (digits < 1) digits = 1;

	int32_t zahlkopie = zahl;

	while (zahlkopie /= 10) neededlength++;

	if (neededlength < digits) neededlength = digits;

	for (uint8_t i = neededlength + vorzeichen; (i - vorzeichen); i--) {
		feld[i - 1] = (zahl % 10) + 0x30;
		zahl       /= 10;
	}

	feld[neededlength + vorzeichen] = '\0';
}

// Initialise LCD
void lcd_init(void) {
	// Grundeinstellungen
	ENPORT &= ~(1 << EN);
	RWPORT &= ~(1 << RW);
	RSPORT &= ~(1 << RS);

	lcd_clear_all_bits();

	ENDDR |= 1 << EN;
	RWDDR |= 1 << RW;
	RSDDR |= 1 << RS;

	#if (LCD_BIT_MODE == 8)
		DB0DDR |= (1 << DB0);
		DB1DDR |= (1 << DB1);
		DB2DDR |= (1 << DB2);
		DB3DDR |= (1 << DB3);
	#endif
	DB4DDR |= (1 << DB4);
	DB5DDR |= (1 << DB5);
	DB6DDR |= (1 << DB6);
	DB7DDR |= (1 << DB7);

	// Initialisierung
	_delay_ms(200);
	uint8_t walking;

	DB5PORT |= 1 << DB5;
	DB4PORT |= 1 << DB4;

	for (walking = 0; walking < 3; walking++) {
		lcd_enable();
		_delay_ms(10);
	}

	// 4-bit- oder 8-bit-Mode
	lcd_clear_all_bits();
	DB5PORT |= 1 << DB5;

	if (LCD_BIT_MODE == 8) DB4PORT |= 1 << DB4;

	lcd_enable();
	lcd_busycheck();

	// Commands according to p.25 in HD44780-datasheet
	lcd_send((1 << 5 | 1 << 3), 0); // Function Set: 4-bit, 2 or 4 lines, 5*8
	lcd_send((1 << 3 | 1 << 2), 0); // Display on/off control: display on, cursor on, blinking off
	lcd_send((1 << 2 | 1 << 1), 0); // Entry mode set: Cursor increment, cursor movement

	// Display löschen
	lcd_clear();
}
