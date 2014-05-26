/*
 * uartstuff.h
 *
 * Einstellungen und Definitionen zur seriellen Kommunikation zwischen Controller und PC
 */

#ifndef UART_H_
#define UART_H_

// Definitionen vom Controller aus gesehen!
#define RXD 		PD0 // Eingang: Empfänger
#define TXD 		PD1	// Ausgang: Sender
#define RTS 		PD2 // Ausgang: Darf die Gegenstelle Daten schicken? (HIGH=nein, LOW=ja)
#define CTS 		PD3 // Eingang: Darf man Daten an Gegenstelle senden? (HIGH=nein, LOW=ja)
#define UART_PORT 	PORTD
#define UART_DDR	DDRD
#define UART_PIN	PIND

#define BAUD 9600

void block_uart_sending(void);
void allow_uart_sending(void);
void uart_cleanup(uint8_t rxtxboth);
void uart_init(uint32_t baud);
uint8_t uart_getc(void);
void uart_gets(char *s);
uint8_t uart_putc(uint8_t c);
void uart_puts (char *s);
void uart_puts_P (const char *s);
uint8_t uart_strings_equal(const char* string1, const char* string2);
uint8_t uart_valid(const char *field);
void uart_shownum(int32_t zahl, uint8_t type);


#endif
