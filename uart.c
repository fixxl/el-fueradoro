/*
 * uart.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

void block_uart_sending(void) {
	UART_PORT |= (1 << RTS);
}

void allow_uart_sending(void) {
	UART_PORT &= ~(1 << RTS);
}

void uart_cleanup(uint8_t rxtxboth) {	// 1:Rx, 2:Tx, 3: komplett
	uint8_t __attribute__((unused)) ttt = 0xAA;

	if (rxtxboth & (1 << 0)) {
		// Flush Receive-Buffer (entfernen evtl. vorhandener ungültiger Werte)
		while (UCSR0A & (1 << RXC0))
			ttt = UDR0;
		// Rücksetzen von Receive und Transmit Complete-Flags
		UCSR0A = (1 << RXC0);
	}

	if (rxtxboth & (1 << 1)) {
		UCSR0A = (1 << TXC0);
	}
}

void uart_init(uint32_t baud) { /* here a simple int will not suffice*/
	uint8_t sreg = SREG;
	cli();

	UART_DDR &= ~(1 << CTS);
	UART_PORT |= (1 << CTS);
	UART_DDR |= (1 << RTS);

	uint32_t baudrate = ((F_CPU + baud * 8) / (baud * 16) - 1); /* as per pg. 133 of the user manual */
	/* Set baud rate */
	UBRR0H = (baudrate >> 8);
	UBRR0L = baudrate;
	/* Enable Receiver and Transmitter */
	UCSR0B = ((1 << RXEN0) | (1 << TXEN0));
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);

	uart_cleanup(3);

	SREG = sreg;
}

/* Zeichen empfangen */
uint8_t uart_getc(void) {
	uint8_t udrcontent;
	uint32_t utimer;
	utimer = TIMEOUTVAL;
	while (!(UCSR0A & (1 << RXC0)) && --utimer); // warten bis Zeichen verfuegbar
	if (!utimer) return '\0';
	block_uart_sending();
	udrcontent = UDR0;
	allow_uart_sending();
	return udrcontent;             // Zeichen aus UDR an Aufrufer zurueckgeben
}

void uart_gets(char *s) {
	uint8_t zeichen = 0;
	char buchstabe = 0xFE;
	uint32_t utimer;
	utimer = TIMEOUTVAL;
	while (buchstabe && --utimer && (zeichen < MAX_ARRAYSIZE)) {
		buchstabe = uart_getc();
		if ((buchstabe == 13 || buchstabe == 10) && zeichen && (s[0] != 0xFF)) buchstabe = '\0';
		s[zeichen] = buchstabe;
		if (s[0] != 0xFF) uart_putc(s[zeichen]);
		zeichen++;
		utimer = TIMEOUTVAL;
		if ((zeichen == 4) && (s[0] == 0xFF)) break;	// Bei Zündbefehl nach 4 Zeichen abbrechen
	}
	s[zeichen] = '\0';
}

uint8_t uart_putc(uint8_t c) {
	uint32_t utimer;
	utimer = TIMEOUTVAL;
	while ((UART_PIN & (1 << CTS)) && --utimer); /* wait till sending is allowed */

	if (utimer) {
		utimer = TIMEOUTVAL;
		while (!(UCSR0A & (1 << UDRE0)) && --utimer);
		switch (c) {
		case 'ä':
			c = 228;
			break;
		case 'Ä':
			c = 196;
			break;
		case 'ö':
			c = 246;
			break;
		case 'Ö':
			c = 214;
			break;
		case 'ü':
			c = 252;
			break;
		case 'Ü':
			c = 220;
			break;
		case 'ß':
			c = 223;
			break;
		default:
			break;
		}
		UDR0 = c; /* sende Zeichen */
		return 0;
	} else {
		return 1;
	}
}

void uart_puts(char *s) {
	uint8_t overflow = 0;
	while (*s && !overflow) { /* so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)" */
		overflow = uart_putc(*s++);
	}
}

void uart_puts_P(const char *s) {
	uint8_t overflow = 0;
	unsigned char c;
	while (!overflow) {
		c = pgm_read_byte(s++);
		if (c == '\0') break;
		overflow = uart_putc(c);
	}
}

uint8_t uart_strings_equal(char* string1, char* string2) {
	while (*string2) {
		if (*string1++ != *string2++) return 0;
	}
	if (*string1) return 0;
	return 1;
}

uint8_t uart_valid(char *field) {
	return ((field[0] == 0xFF) && (field[1] > 0) && (field[1] < 31) && (field[2] > 0) && (field[2] < 17)
			&& (field[3] == crc8(crc8(0, field[1]), field[2])));
}

void uart_shownum(int32_t zahl, uint8_t type) {
	uint8_t i = 0, lauf = 1;
	uint32_t bits = 1;
	int32_t temp = zahl;
	char zwischenspeicher[33] = { 0 };

	switch (type) {
	// Binärdarstellung
	case 'b':
	case 'B': {
		while (bits < zahl) {
			bits <<= 1;
		}
		if (bits < 128) bits = 128;
		else if (bits < 32768) bits = 32768;
		else bits = 2147483648;

		while (bits) {
			uart_putc((zahl & bits) ? '1' : '0');
			bits >>= 1;
		}
		break;
	}

		// Hexadezimaldarstellung
	case 'h':
	case 'H': {
		while (temp || !i) {
			zwischenspeicher[i] = temp & 0x0F;
			temp >>= 4;
			i++;
		}
		if (i % 2) i++;
		temp = zahl;
		for (lauf = i; lauf > 0; lauf--) {
			if (zwischenspeicher[lauf - 1] > 9) uart_putc((zwischenspeicher[lauf - 1] % 10) + 'A');
			else uart_putc(zwischenspeicher[lauf - 1] + '0');
		}
		break;
	}

		// Standarddarstellung (=dezimal)
	default: {
		while (temp /= 10) {
			lauf++;
		}

		if (zahl < 0) {
			uart_puts_P(PSTR("-"));
			zahl = -zahl;
		}

		for (uint8_t j = lauf; j > 0; j--) {
			zwischenspeicher[j - 1] = ((zahl % 10) + 0x30);
			zahl /= 10;
		}

		for (uint8_t j = 0; j < lauf; j++) {
			uart_putc(zwischenspeicher[j]);
		}
		break;
	}
	}
}
