/*
 * uart.h
 *
 * Settings and definitions concerning the serial communication between controller and PC
 */

#ifndef UART_H_
#define UART_H_

// Definitions (from controller-perspective!)
#define RXDPORT   D
#define RXD     0   // Receiver input
#define TXDPORT   D
#define TXD     1 // Transmitter output
#define RTSPORT   D
#define RTS     2   // Output: Controller ready to receive data? (HIGH=no, LOW=yes)
#define CTSPORT   D
#define CTS     3   // Input: May controller transmit data, other side ready? (HIGH=no, LOW=yes)

// Set baud rate
#define BAUD 9600

// Use RTS/CTS flow control?
#define RTSCTSFLOW 1

// Does upper case/lower case matter?
#define CASE_SENSITIVE 0


// DO NOT CHANGE ANYTHING BELOW THIS LINE

#define RXD_PORT  PORT(RXDPORT)
#define RXD_DDR   DDR(RXDPORT)
#define RXD_PIN   PIN(RXDPORT)

#define TXD_PORT  PORT(TXDPORT)
#define TXD_DDR   DDR(TXDPORT)
#define TXD_PIN   PIN(TXDPORT)

#define RTS_PORT  PORT(RTSPORT)
#define RTS_DDR   DDR(RTSPORT)
#define RTS_PIN   PIN(RTSPORT)

#define CTS_PORT  PORT(CTSPORT)
#define CTS_DDR   DDR(CTSPORT)
#define CTS_PIN   PIN(CTSPORT)

#define UART_TIMEOUTVAL (F_CPU)

void block_uart_sending(void);
void allow_uart_sending(void);

void uart_init(uint32_t baud);
uint8_t uart_getc(void);
uint8_t uart_gets(char *s);
uint8_t uart_putc(uint8_t c);
void uart_puts (char *s);
void uart_puts_P (const char *s);
uint8_t uart_strings_equal(const char *string1, const char *string2);
void uart_shownum(int32_t zahl, uint8_t type);
uint8_t uart_lower_case(char letter);


#endif
