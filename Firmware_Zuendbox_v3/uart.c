/*
 * uart.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

void block_uart_sending(void) {
    #if RTSCTSFLOW
        RTS_PORT |= (1 << RTS);
    #endif
}

void allow_uart_sending(void) {
    #if RTSCTSFLOW
        RTS_PORT &= ~(1 << RTS);
    #endif
}

// Initialise UART with given baud rate
void uart_init(uint32_t baud) {
    uint8_t sreg = SREG;
    cli();

    #if RTSCTSFLOW
        CTS_PORT |= (1 << CTS);
        CTS_DDR  &= ~(1 << CTS);
        RTS_DDR  |= (1 << RTS);
    #endif

    uint32_t baudrate = ((F_CPU + (baud << 3)) / (baud << 4) - 1);
    /* Set baud rate */
    UBRR0H = (baudrate >> 8);
    UBRR0L = baudrate;
    /* Enable Receiver and Transmitter */
    UCSR0B = ((1 << RXEN0) | (1 << TXEN0));
    /* Set frame format: 8data, 1stop bit */
    UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);

    // Enable pullup on RX-line to avoid floating
    RXD_PORT |= (1 << RXD);
    
    SREG = sreg;
}

// Receive char
uint8_t uart_getc(void) {
    uint8_t  udrcontent;
    uint32_t utimer = UART_TIMEOUTVAL;

    while (--utimer && !(UCSR0A & (1 << RXC0))) ; // wait until char available or timeout

    if (!utimer) return '\0';

    block_uart_sending();
    udrcontent = UDR0;
    allow_uart_sending();
    return udrcontent;
}

// Receive string
uint8_t uart_gets(char *s) {
    uint8_t zeichen   = 0;
    char    buchstabe = 0xFE;

    while (buchstabe && (zeichen < MAX_ARRAYSIZE - 1)) {
        buchstabe = uart_getc();
        #if !CASE_SENSITIVE
            buchstabe = uart_lower_case(buchstabe);
        #endif

        if ((buchstabe == 13) || (buchstabe == 10)) buchstabe = '\0';  // ENTER means "end of string"

        if ((buchstabe == 8) || (buchstabe == 127)) {                  // In case of backspace
            if (!zeichen) {
                s[0] = '\0';
                return 0;
            }
            else {
                zeichen--;
                uart_puts_P(PSTR("\033[1D \033[1D"));
            }
        }
        else {
            s[zeichen] = buchstabe; // Write char to array
            uart_putc(buchstabe);

            if (buchstabe) zeichen++; }
    }

    if (zeichen) uart_puts_P(PSTR("\n\r"));

    return zeichen; // Return the number of chars received - which equals the index of terminating '\0'
}

// Transmit char
uint8_t uart_putc(uint8_t c) {
    uint32_t utimer;
    utimer = UART_TIMEOUTVAL;
    #if RTSCTSFLOW
        while (--utimer && (CTS_PIN & (1 << CTS))) ; /* wait till sending is allowed */
    #endif

    if (utimer) {
        utimer = UART_TIMEOUTVAL;

        while (--utimer && !(UCSR0A & (1 << UDRE0))) ;

        switch (c) {
             /*case '�': {
                  c = 228;
                  break;
              }

             case '�': {
                  c = 196;
                  break;
              }

             case '�': {
                  c = 246;
                  break;
              }

             case '�': {
                  c = 214;
                  break;
              }

             case '�': {
                  c = 252;
                  break;
              }

             case '�': {
                  c = 220;
                  break;
              }

             case '�': {
                  c = 223;
                  break;
              }*/

             default: {
                  break;
              }
        }

        UDR0 = c;
        return 0;
    }
    else {
        return 1;
    }
}

// Transmit string
void uart_puts(char *s) {
    uint8_t overflow = 0;

    while (*s && !overflow) {
        overflow = uart_putc(*s++);
    }
}

// Transmit string from flash memory (String constants)
void uart_puts_P(const char *s) {
    uint8_t       overflow = 0;
    unsigned char c;

    while (!overflow) {
        c = pgm_read_byte(s++);

        if (c == '\0') break;

        overflow = uart_putc(c);
    }
}

// Compare strings if equal (1) or not (0)
uint8_t uart_strings_equal(const char *string1, const char *string2) {
    while (*string2)
        if (*string1++ != *string2++) return 0;

    if (*string1) return 0;

    return 1;
}

// Print number in decimal, hexadecimal or binary format to console
void uart_shownum(int32_t zahl, uint8_t type) {
    uint8_t  i    = 0, lauf = 1;
    uint32_t bits = 1, zahltemp = (zahl < 0) ? -zahl : zahl;
    char     zwischenspeicher[33] = { 0 };

    switch (type) {
         // Binary
         case 'b':
         case 'B': {
              if (zahltemp < 256) bits = 128;
              else if (zahltemp < 65535) bits = 32768;
              else   bits = 2147483648;

              while (bits) {
                  uart_putc((zahl & bits) ? '1' : '0');
                  bits >>= 1;
              }

              break;
          }

         // Hex
         case 'h':
         case 'H': {
              while (zahl || !i) {
                  zwischenspeicher[i++] = zahl & 0x0F;
                  zahl >>= 4;
              }

              if (i % 2) i++;

              for (lauf = i; lauf; lauf--) {
                  if (zwischenspeicher[lauf - 1] > 9) uart_putc((zwischenspeicher[lauf - 1] - 10) + 'A'); else uart_putc(
                         zwischenspeicher[lauf - 1] + '0');
              }

              break;
          }

         // Decimal
         default: {
              if (!zahl) {
                  uart_puts_P(PSTR("0"));
                  return;
              }

              if (zahl < 0) {
                  i    = 1;
                  zahl = -zahl;
              }

              lauf = 0;

              while (zahl && (lauf < 33)) {
                  zwischenspeicher[lauf++] = zahl % 10;
                  zahl /= 10;
              }

              if (i) uart_puts_P(PSTR("-"));

              while (lauf) {
                  uart_putc(zwischenspeicher[lauf - 1] + '0');
                  lauf--;
              }

              break;
          }
    }
}

// Ensure lower case letters
uint8_t uart_lower_case(char letter) {
    // if (((letter >= 'A') && (letter <= 'Z')) || (letter == '�') || (letter == '�') || (letter == '�')) letter |= 0x20;
    if (((letter >= 'A') && (letter <= 'Z')) || (letter == 0xC4) || (letter == 0xD6) || (letter == 0xDC)) letter |= 0x20;
    return letter;
}
