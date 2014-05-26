/*
 * uart.h
 *
 * Settings and definitions concerning the serial communication between controller and PC
 */

#ifndef UART_H_
#define UART_H_

// Definitionen vom Controller aus gesehen!
#define RXD 		PD0 // Receiver input
#define TXD 		PD1	// Transmitter output
#define RTS 		PD2 // Output: Controller ready to receive data? (HIGH=no, LOW=yes)
#define CTS 		PD3 // Input: May controller transmit data, other side ready? (HIGH=no, LOW=yes)
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
